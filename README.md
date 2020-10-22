<p align="center">
    <img alt="Ion Banner Graphic" src="https://repository-images.githubusercontent.com/233767923/b799f780-d9cf-11ea-9d70-2597e4821c9e" />
</p>

#### ionlang

Ion is a general purpose programming language, currently undergoing active development.

[Join our Discord server](https://discord.gg/H3eMUXp)

#### Syntax

```rust
module global;

extern puts(i8* msg) -> i32;

extern printf(i8* format, ...) -> i32;

struct Animal {
  i8* name;

  ui8 age;

  fn sayName() { puts(this.name); }
}

struct Dog extends Animal {
  fn bark() -> void { printf("%s barks woof woof!", this.name); }

  fn computeAgeInHumanYears() -> ui8 { return this.age * 7; }
}

fn main(i32 argc) -> i32 {
  let mikewazowski = Dog{
    "Mikewazowski",
    7
  };

  mikewazowski.bark();

  printf("%s's age is %s (%s in human years).",
    mikewazowski.name,
    mikewazowski.age,
    mikewazowski.computeAgeInHumanYears());

  return 0;
}
```
