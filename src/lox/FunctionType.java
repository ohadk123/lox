package lox;

public enum FunctionType {
    NONE("global"),
    FUNCTION("function"),
    METHOD("method");

    private final String name;

    FunctionType(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        return this.name;
    }
}