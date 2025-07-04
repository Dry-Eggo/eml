
## Assignment

In EML, Two methods are provided to bind values to identifiers:
	* ':=' = Mutable Binding
	* '='  = Constant Binding

Eml doesn't enforce immutability by default but it encourages it by making the popular operator for assignment
the constant operator

```eml
// Mutable
name := "Dry";
// Constant
PI    = 3.14;
```
