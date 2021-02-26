###############################################################################
#cFlags= -c -O3 -DNDEBUG -static -fno-strict-aliasing 
###############################################################################
cFlags= -c -O3 -funroll-all-loops -w -I. -I./inc
###############################################################################
oFiles=	mod.o modbin.o moddef.o bmc.o cex.o c2v.o coi.o f2p.o p2bp.o q0cnf.o q2nt.o satis.o tbdmm.o tlf.o bp2q.o bp2t.o clk.o dmc.o main.o prep.o q1rd.o qbf.o tbdbw.o tbdop.o tree.o bp2s.o spec.o ltl.o ctl.o conv2.o conv.o f2cnf.o ctlr2.o crspec.o ctlrlin.o ctlrlin1.o epc.o
###############################################################################
.c.o:
	gcc $(cFlags) $<
###############################################################################
verds:	$(oFiles)
	gcc -o verds $(oFiles) -lm
###############################################################################
