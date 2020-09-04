; ModuleID = 'test'
source_filename = "test"

define void @foobar() {
entry:
  br i1 true, label %tmp_block_2, label %tmp_block_1

tmp_block_2:                                      ; preds = %entry
  br label %tmp_block_1

tmp_block_1:                                      ; preds = %entry, %tmp_block_2
}
