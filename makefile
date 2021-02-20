gift_wrapping: clean gift_wrapping.c
	@gcc -g gift_wrapping.c -lm -o gift_wrapping.out

clean:
	@rm -rf gift_wrapping.out .points .vertices