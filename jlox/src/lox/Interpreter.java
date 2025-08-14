package lox;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
    final Environment globals = new Environment();
    private Environment environment = globals;
    private final Map<Expr, Integer> locals = new HashMap<>();

    Interpreter() {
        globals.define("clock", NativeFunction.clock);
        globals.define("read", NativeFunction.read);
    }

    void interpret(List<Stmt> statements) {
        try {
            for (Stmt statement : statements) execute(statement);
        } catch (RuntimeError error) {
            Lox.runtimeError(error);
        }
    }

//************************************
// Expression Evaluations
//************************************
    private Object evaluate(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
        Object left = evaluate(expr.left);
        Object right = evaluate(expr.right);

        switch (expr.operator.type) {
            case MINUS:
                checkNumberOperands(expr.operator, left, right);
                return (double) left - (double) right;
            case STAR:
                checkNumberOperands(expr.operator, left, right);
                return (double) left * (double) right;
            case SLASH:
                checkNumberOperands(expr.operator, left, right);
                return (double) left / (double) right;
            case PLUS:
                if (left instanceof Double && right instanceof Double) return (double) left + (double) right;
                if (left instanceof String && right instanceof String) return (String) left + (String) right;
                if (left instanceof String) return (String) left + right.toString();
                if (right instanceof String) return left.toString() + (String) right;
                throw new RuntimeError(expr.operator, "Operands must be two numbers or two strings.");
            case GREATER:
            case GREATER_EQUAL:
            case LESS:
            case LESS_EQUAL:
            case EQUAL_EQUAL:
            case BANG_EQUAL:
                return compare(expr.operator, left, right);
        }

        return null;
    }

    @Override
    public Object visitGroupingExpr(Expr.Grouping expr) {
        return evaluate(expr.expression);
    }

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
        return expr.value;
    }

    @Override
    public Object visitLogicalExpr(Expr.Logical expr) {
        Object left = evaluate(expr.left);

        if (expr.operator.type == TokenType.OR) if (isTruthy(left)) return left;
        if (expr.operator.type == TokenType.AND) if (!isTruthy(left)) return left;

        return evaluate(expr.right);
    }

    @Override
    public Object visitSetExpr(Expr.Set expr) {
        Object object = evaluate(expr.object);

        if (!(object instanceof LoxInstance)) throw new RuntimeError(expr.name, "Only instances have fields.");

        Object value = evaluate(expr.value);
        ((LoxInstance) object).set(expr.name, value);
        return value;
    }

    @Override
    public Object visitThisExpr(Expr.This expr) {
        lookUpVariable(expr.keyword, expr);
        return null;
    }

    @Override
    public Object visitSuperExpr(Expr.Super expr) {
        int distance = locals.get(expr);

        LoxClass superclass = (LoxClass) environment.getAt(distance, "super");
        LoxInstance object = (LoxInstance) environment.getAt(distance - 1, "this");
        LoxFunction method = superclass.findMethod(expr.method.lexeme);

        if (method == null) throw new RuntimeError(expr.method, "Undefined property '" + expr.method.lexeme + "'.");

        return method.bind(object);
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
        Object right = evaluate(expr.right);

        switch (expr.operator.type) {
            case MINUS:
                checkNumberOperand(expr.operator, right);
                return -(double) right;
            case BANG:
                return !isTruthy(right);
        }

        return null;
    }

    @Override
    public Object visitCallExpr(Expr.Call expr) {
        Object callee = evaluate(expr.callee);

        List<Object> arguments = new ArrayList<>();
        for (Expr arg : expr.arguments) arguments.add(evaluate(arg));

        if (!(callee instanceof LoxCallable function)) throw new RuntimeError(expr.paren, "Can only call functions and classes.");
        if (arguments.size() != function.arity()) throw new RuntimeError(expr.paren, "Expected " + function.arity() + " arguments, but got " + arguments.size() + ".");
        return function.call(this, arguments);
    }

    @Override
    public Object visitGetExpr(Expr.Get expr) {
        Object object = evaluate(expr.object);
        if (object instanceof LoxInstance) return ((LoxInstance) object).get(expr.name);

        throw new RuntimeError(expr.name, "Only instances have properties.");
    }

    @Override
    public Object visitVariableExpr(Expr.Variable expr) {
        return lookUpVariable(expr.name, expr);
    }

    @Override
    public Object visitAssignExpr(Expr.Assign expr) {
        Object value = evaluate(expr.value);

        Integer distance = locals.get(expr);
        if (distance != null) environment.assignAt(distance, expr.name, value);
        else globals.assign(expr.name, value);
        return value;
    }


//************************************
// Statement Executions
//************************************
    void execute(Stmt stmt) {
        stmt.accept(this);
    }

    void executeBlock(List<Stmt> statements, Environment environment) {
        Environment previous = this.environment;
        try {
            this.environment = environment;
            for (Stmt statement : statements) execute(statement);
        } finally {
            this.environment = previous;
        }
    }

    @Override
    public Void visitBlockStmt(Stmt.Block stmt) {
        executeBlock(stmt.statements, new Environment(environment));
        return null;
    }

    @Override
    public Void visitClassStmt(Stmt.Class stmt) {
        Object superclass = null;
        if (stmt.superclass != null) {
            superclass = evaluate(stmt.superclass);
            if (!(superclass instanceof LoxClass))
                throw new RuntimeError(stmt.superclass.name, "Superclass must be a class.");
        }

        environment.define(stmt.name.lexeme, null);

        if (stmt.superclass != null) {
            environment = new Environment(environment);
            environment.define("super", superclass);
        }

        Map<String, LoxFunction> methods = new HashMap<>();
        for (Stmt.Function method : stmt.methods) {
            LoxFunction function = new LoxFunction(method, environment, stmt.name.lexeme.equals("init"));
            methods.put(method.name.lexeme, function);
        }

        LoxClass klass = new LoxClass(stmt.name.lexeme, (LoxClass) superclass, methods);
        if (superclass != null) environment = environment.enclosing;
        environment.assign(stmt.name, klass);
        return null;
    }

    @Override
    public Void visitExpressionStmt(Stmt.Expression stmt) {
        evaluate(stmt.expression);
        return null;
    }

    @Override
    public Void visitFunctionStmt(Stmt.Function stmt) {
        globals.define(stmt.name.lexeme, new LoxFunction(stmt, environment, false));
        return null;
    }

    @Override
    public Void visitIfStmt(Stmt.If stmt) {
        if (isTruthy(evaluate(stmt.condition))) execute(stmt.thenBranch);
        else if (stmt.elseBranch != null) execute(stmt.elseBranch);
        return null;
    }

    @Override
    public Void visitPrintStmt(Stmt.Print stmt) {
        Object value = evaluate(stmt.expression);
        System.out.println(stringify(value));
        return null;
    }

    @Override
    public Void visitReturnStmt(Stmt.Return stmt) {
        Object value = null;
        if (stmt.value != null) value = evaluate(stmt.value);
        throw new Return(value);
    }

    @Override
    public Void visitVarStmt(Stmt.Var stmt) {
        Object value = null;
        if (stmt.initializer != null) value = evaluate(stmt.initializer);

        environment.define(stmt.name.lexeme, value);
        return null;
    }

    @Override
    public Void visitWhileStmt(Stmt.While stmt) {
        while (isTruthy(evaluate(stmt.condition))) execute(stmt.body);
        return null;
    }


//************************************
// utility Functions
//************************************

    /**
     * Check if lox object is "Truthy".
     * @param object the object to check
     * @return true if the object is truthy, false otherwise.
     */
    private boolean isTruthy(Object object) {
        if (object == null) return false;
        if (object instanceof Boolean) return (boolean) object;
        return true;
    }

    /**
     * Check if two lox objects are equal.
     * @param a first object
     * @param b second object
     * @return true if both objects are equal, false otherwise.
     */
    private boolean isEqual(Object a, Object b) {
        if (a == null && b == null) return true;
        if (a == null) return false;

        return a.equals(b);
    }

    /**
     * Convert a lox object to a string representation.
     * @param object the object to convert
     * @return a string representation of the object
     */
    private String stringify(Object object) {
        if (object == null) return "nil";

        if (object instanceof Double) {
            String text = object.toString();
            if (text.endsWith(".0")) text = text.substring(0, text.length() - 2);
            return text;
        }

        return object.toString();
    }

    /**
     * Check if the operand is a number.
     * @param operator the operator token used in the expression
     * @param operand the operand to check
     * @throws RuntimeError if the operand is not a number
     */
    private void checkNumberOperand(Token operator, Object operand) {
        if (operand instanceof Double) return;
        throw new RuntimeError(operator, "Operand must be a number.");
    }

    /**
     * Check if both operands are numbers.
     * @param operator the operator token used in the expression
     * @param left the left operand
     * @param right the right operand
     * @throws RuntimeError if either operand is not a number
     */
    private void checkNumberOperands(Token operator, Object left, Object right) {
        if (left instanceof Double && right instanceof Double) return;
        throw new RuntimeError(operator, "Operands must be numbers.");
    }

    /**
     * Compare two lox objects based on the operator type.
     * @param token the operator token
     * @param a the first operand
     * @param b the second operand
     * @return true if the comparison is true, false otherwise
     */
    private boolean compare(Token token, Object a, Object b) {
        TokenType type = token.type;

        if (a instanceof Double && b instanceof Double) {
            double da = (double) a;
            double db = (double) b;
            return switch (type) {
                case GREATER -> da > db;
                case GREATER_EQUAL -> da >= db;
                case LESS -> da < db;
                case LESS_EQUAL -> da <= db;
                case EQUAL_EQUAL -> da == db;
                case BANG_EQUAL -> da != db;
                default -> throw new RuntimeException("type must be a kind of comparison");
            };
        }

        if (a instanceof String && b instanceof String) {
            int res = C_strcmp((String) a, (String) b);
            return switch (type) {
                case GREATER -> res > 0;
                case GREATER_EQUAL -> res >= 0;
                case LESS -> res < 0;
                case LESS_EQUAL -> res <= 0;
                case EQUAL_EQUAL -> res == 0;
                case BANG_EQUAL -> res != 0;
                default -> throw new RuntimeException("type must be a kind of comparison");
            };
        }

        switch (type) {
            case GREATER:
            case GREATER_EQUAL:
            case LESS:
            case LESS_EQUAL:
                throw new RuntimeError(token, "Operands must be two numbers or two strings.");
            case EQUAL_EQUAL:
                return isEqual(a, b);
            case BANG_EQUAL:
                return !isEqual(a, b);
            default:
                throw new RuntimeException("Unreachable!");
        }
    }

    /**
     * Compare two strings lexicographically.
     * This is a C-style strcmp implementation.
     * @param a first string
     * @param b second string
     * @return negative if a < b, positive if a > b, 0 if a == b
     */
    private int C_strcmp(String a, String b) {
        int n = Math.min(a.length(), b.length());
        for (int i = 0; i < n; i++) {
            int cmp = a.charAt(i) - b.charAt(i);
            if (cmp != 0) return cmp;
        }

        if (a.length() > b.length()) return a.charAt(n);
        if (b.length() > a.length()) return b.charAt(n);
        return 0;
    }

    public void resolve(Expr expr, int depth) {
        locals.put(expr, depth);
    }

    private Object lookUpVariable(Token name, Expr expr) {
        Integer distance = locals.get(expr);
        if (distance != null) return environment.getAt(distance, name.lexeme);
        else return globals.get(name);
    }
}
