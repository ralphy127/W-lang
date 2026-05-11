# Interpreter (Tree-walk Evaluator)

The interpreter executes the code by traversing the AST. It's a "tree-walk" interpreter, meaning it recursively visits each node of the AST and performs the corresponding action. For example, when it visits an addition node, it evaluates the left and right children and returns their sum.

The core of the interpreter is the `evaluate` method, which dispatches to a `visit` method for each AST node type. This is an implementation of the Visitor design pattern.
