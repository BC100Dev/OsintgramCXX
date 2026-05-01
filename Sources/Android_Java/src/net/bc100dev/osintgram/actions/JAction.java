package net.bc100dev.osintgram.actions;

public abstract class JAction {

    protected final String[] args;

    public JAction(String[] args) {
        this.args = args;
    }

    public abstract void executeAction();

}
