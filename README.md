## cppwee

cppwee is a c++ workflow execution engine(WEE). It's designed to be a library instead of an application, which means you can embed it in your c++ application to control execution flow.

In an execution flow, cppwee use `Statement` to represent the nodes in the flow. And it use `Arguments` to represent the data passed between statments. You can implement your own statements and arguments so that cppwee support different kind usage.

## TODO

* implement Windows process statement
* write a front-end DSL 
