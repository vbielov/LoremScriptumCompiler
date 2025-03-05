	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.data
	.weak	yesStr
yesStr:
	.asciz	"Yes"

	.weak	noStr
noStr:
	.asciz	"No"

