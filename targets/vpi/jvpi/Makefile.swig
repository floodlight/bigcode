#
# Static rules for SWIG generation
#
all: swig

PACKAGE=com.bigswitch.vpi.native_.swig
OUTDIR=com/bigswitch/vpi/native_/swig
swigclean:
	@rm -f VPI_wrap.c
	@rm -f $(OUTDIR)/*.java
	@rm -f $(OUTDIR)/*.class

swig: swigclean
	swig -java -package $(PACKAGE) -outdir $(OUTDIR) VPI.i

