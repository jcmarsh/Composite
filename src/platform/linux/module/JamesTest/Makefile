default:
	gcc -g -o kern_entry64 kern_entry64.S

test:
	gcc -g -o stack_test stack_test.S
	gcc -g -o stack2_test stack2_test.S
	gcc -g -o define_test define_test.S
	gcc -g -o subq_test subq_test.S
	gcc -g -o hello hello.S
	gcc -g -o jmp_test jmp_test.S

clean:
	rm stack_test define_test subq_test hello jmp_test kern_entry64