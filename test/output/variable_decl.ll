; ModuleID = 'test'
source_filename = "test"

define void @foobar() {
entry:
  %foo = alloca i1
  store i1 true, i1* %foo
}
