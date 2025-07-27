package lox;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public abstract class NativeFunction implements LoxCallable {
    @Override
    public String toString() {
        return "<native fn>";
    }

    static NativeFunction clock = new NativeFunction() {
        @Override
        public int arity() {
            return  0;
        }

        @Override
        public Object call(Interpreter interpreter, List<Object> arguments) {
            return (double) System.currentTimeMillis() / 1000.0;
        }
    };

    static NativeFunction read = new NativeFunction() {
        @Override
        public int arity() { return 1; }

        @Override
        public Object call(Interpreter interpreter, List<Object> arguments) {
            try {
                return Files.readString(Paths.get((String) arguments.getFirst()));
            } catch (Exception e) {
                return null;
            }
        }
    };
}
