include ../build-aux/platform.mak
include ../build-aux/compiler.mak
include ../build-aux/version.mak
include ../build-aux/dirs.mak

OPTLEVEL?=$(DEFAULT_OPTLEVEL)
CFLAGS?=$(OPTLEVEL)

# TODO: Better detection of the proper subdirectory here!
ifneq ($(shell echo $(HOST) | grep -E '^i[012346789]*86-'),)
  ARCH_SUBDIR:=arch/i387
  ARCH_MACHINE_HEADERS:=$(ARCH_SUBDIR)/machine/npx.h
endif
ifneq ($(shell echo $(HOST) | grep -E '^x86_64-'),)
  ARCH_SUBDIR:=arch/x86_64
  ARCH_MACHINE_HEADERS:=$(ARCH_SUBDIR)/machine/fpu.h
endif

ARCH_MACHINE_HEADERS:=$(ARCH_MACHINE_HEADERS) $(ARCH_SUBDIR)/machine/fenv.h

ARCH_SRCS=\
e_acos.S \
e_asin.S \
e_atan2.S \
e_expf.S \
e_exp.S \
e_fmod.S \
e_log10f.S \
e_log10.S \
e_log2f.S \
e_log2.S \
e_logf.S \
e_log.S \
e_remainderf.S \
e_remainder.S \
e_scalbf.S \
e_scalb.S \
e_sqrtf.S \
e_sqrt.S \
fabs.S \
fenv.c \
flt_rounds.S \
fpgetmask.S \
fpgetprec.S \
fpgetround.S \
fpgetsticky.S \
fpsetmask.S \
fpsetprec.S \
fpsetround.S \
fpsetsticky.S \
lrint.S \
s_atanf.S \
s_atan.S \
s_ceilf.S \
s_ceil.S \
s_copysignf.S \
s_copysign.S \
s_cosf.S \
s_cos.S \
s_finitef.S \
s_finite.S \
s_floorf.S \
s_floor.S \
s_ilogbf.S \
s_ilogbl.S \
s_ilogb.S \
s_log1pf.S \
s_log1p.S \
s_logbf.S \
s_logbl.S \
s_logb.S \
s_rintf.S \
s_rint.S \
s_scalbnf.S \
s_scalbn.S \
s_significandf.S \
s_significand.S \
s_sinf.S \
s_sin.S \
s_tanf.S \
s_tan.S \

COMMON_SRCS+=\
b_exp.c \
b_log.c \
b_tgamma.c \
compat_frexp_ieee754.c \
compat_ldexp_ieee754.c \
e_acos.c \
e_acosf.c \
e_acosh.c \
e_acoshf.c \
e_asin.c \
e_asinf.c \
e_atan2.c \
e_atan2f.c \
e_atanh.c \
e_atanhf.c \
e_cosh.c \
e_coshf.c \
e_exp.c \
e_expf.c \
e_fmod.c \
e_fmodf.c \
e_hypot.c \
e_hypotf.c \
e_j0.c \
e_j0f.c \
e_j1.c \
e_j1f.c \
e_jn.c \
e_jnf.c \
e_lgammaf_r.c \
e_lgamma_r.c \
e_log10.c \
e_log10f.c \
e_log2.c \
e_log2f.c \
e_log.c \
e_logf.c \
e_pow.c \
e_powf.c \
e_remainder.c \
e_remainderf.c \
e_rem_pio2.c \
e_rem_pio2f.c \
e_scalb.c \
e_scalbf.c \
e_sinh.c \
e_sinhf.c \
e_sqrt.c \
e_sqrtf.c \
fpclassifyd_ieee754.c \
fpclassifyf_ieee754.c \
fpclassifyl.c \
fpclassifyl_ieee754.c \
isfinited_ieee754.c \
isfinitef_ieee754.c \
isfinitel.c \
isfinitel_ieee754.c \
isinfd_ieee754.c \
isinff_ieee754.c \
isinfl.c \
isinfl_ieee754.c \
isnand_ieee754.c \
isnanf_ieee754.c \
isnanl.c \
isnanl_ieee754.c \
k_cos.c \
k_cosf.c \
k_rem_pio2.c \
k_rem_pio2f.c \
k_sin.c \
k_sinf.c \
k_standard.c \
k_tan.c \
k_tanf.c \
llrint.c \
llrintf.c \
llround.c \
llroundf.c \
lrint.c \
lrintf.c \
lround.c \
lroundf.c \
modf_ieee754.c \
nan.c \
nanf.c \
nanl.c \
s_asinh.c \
s_asinhf.c \
s_atan.c \
s_atanf.c \
s_cbrt.c \
s_cbrtf.c \
s_ceil.c \
s_ceilf.c \
s_copysign.c \
s_copysignf.c \
s_copysignl.c \
s_cos.c \
s_cosf.c \
s_erf.c \
s_erff.c \
s_exp2.c \
s_exp2f.c \
s_expm1.c \
s_expm1f.c \
s_fabsf.c \
s_fabsl.c \
s_fdim.c \
s_finite.c \
s_finitef.c \
s_floor.c \
s_floorf.c \
s_fmax.c \
s_fmaxf.c \
s_fmaxl.c \
s_fmin.c \
s_fminf.c \
s_fminl.c \
s_frexp.c \
s_frexpf.c \
signbitd_ieee754.c \
signbitf_ieee754.c \
signbitl.c \
s_ilogb.c \
s_ilogbf.c \
s_ilogbl.c \
s_isinff.c \
s_isnanf.c \
s_ldexp.c \
s_ldexpf.c \
s_lib_version.c \
s_log1p.c \
s_log1pf.c \
s_logb.c \
s_logbf.c \
s_logbl.c \
s_matherr.c \
s_modf.c \
s_modff.c \
s_nextafter.c \
s_nextafterf.c \
s_nextafterl.c \
s_nexttoward.c \
s_remquo.c \
s_remquof.c \
s_rint.c \
s_rintf.c \
s_round.c \
s_roundf.c \
s_scalbn.c \
s_scalbnf.c \
s_scalbnl.c \
s_signgam.c \
s_significand.c \
s_significandf.c \
s_sin.c \
s_sinf.c \
s_tan.c \
s_tanf.c \
s_tanh.c \
s_tanhf.c \
s_tgammaf.c \
s_trunc.c \
s_truncf.c \
w_acos.c \
w_acosf.c \
w_acosh.c \
w_acoshf.c \
w_asin.c \
w_asinf.c \
w_atan2.c \
w_atan2f.c \
w_atanh.c \
w_atanhf.c \
w_cosh.c \
w_coshf.c \
w_drem.c \
w_dremf.c \
w_exp.c \
w_expf.c \
w_fmod.c \
w_fmodf.c \
w_gamma.c \
w_gammaf.c \
w_gammaf_r.c \
w_gamma_r.c \
w_hypot.c \
w_hypotf.c \
w_j0.c \
w_j0f.c \
w_j1.c \
w_j1f.c \
w_jn.c \
w_jnf.c \
w_lgamma.c \
w_lgammaf.c \
w_lgammaf_r.c \
w_lgamma_r.c \
w_log10.c \
w_log10f.c \
w_log2.c \
w_log2f.c \
w_log.c \
w_logf.c \
w_pow.c \
w_powf.c \
w_remainder.c \
w_remainderf.c \
w_scalb.c \
w_scalbf.c \
w_sinh.c \
w_sinhf.c \
w_sqrt.c \
w_sqrtf.c \

COMPLEX_SRCS+=\
cabs.c \
cabsf.c \
cacos.c \
cacosf.c \
cacosh.c \
cacoshf.c \
carg.c \
cargf.c \
casin.c \
casinf.c \
casinh.c \
casinhf.c \
catan.c \
catanf.c \
catanh.c \
catanhf.c \
ccos.c \
ccosf.c \
ccosh.c \
ccoshf.c \
cephes_subr.c \
cephes_subrf.c \
cexp.c \
cexpf.c \
cimag.c \
cimagf.c \
cimagl.c \
clog.c \
clogf.c \
conj.c \
conjf.c \
conjl.c \
cpow.c \
cpowf.c \
cproj.c \
cprojf.c \
cprojl.c \
creal.c \
crealf.c \
creall.c \
csin.c \
csinf.c \
csinh.c \
csinhf.c \
csqrt.c \
csqrtf.c \
ctan.c \
ctanf.c \
ctanh.c \
ctanhf.c \

CFLAGS:=$(CFLAGS) -std=gnu99 -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -I include -I src -I $(ARCH_SUBDIR)

# TODO: Figure out whether these are the defines that we want to pass.
CPPFLAGS:=$(CPPFLAGS) -D__is_sortix_libm -D_MULTI_LIBM -D_POSIX_MODE

ARCH_SRCS:=$(addprefix $(ARCH_SUBDIR)/,$(ARCH_SRCS))
COMMON_SRCS:=$(addprefix src/,$(COMMON_SRCS))
COMPLEX_SRCS:=$(addprefix complex/,$(COMPLEX_SRCS))
SRCS:=$(ARCH_SRCS) $(COMMON_SRCS) $(COMPLEX_SRCS)

OBJS:=$(SRCS)
OBJS:=$(OBJS:.c=.o)
OBJS:=$(OBJS:.S=.o)

BINS:=libm.a

all: libs

libs: $(BINS)

.PHONY: libs headers clean install install-include-dirs install-headers \
        install-libm-dirs install-libm install-libs

# TODO: Do not pick up the i387 asm version, it is incorrect
arch/i387/s_modf.o: src/s_modf.c

#.if (${MACHINE_ARCH} == "i386")
## XXX this gets miscompiled. There should be a better fix.
#COPTS.s_tanh.c+= -O0
#.endif

headers:

libm.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.S
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

clean:
	rm -f $(BINS) $(OBJS) $(ARCH_SUBDIR)/*.o src/*.o */*.o arch/*/*.o

# Installation into sysroot
install: install-headers install-libm

install-include-dirs: headers
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	mkdir -p $(DESTDIR)$(INCLUDEDIR)/machine

install-headers: install-include-dirs headers
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)
	cp -v $(ARCH_MACHINE_HEADERS) $(DESTDIR)$(INCLUDEDIR)/machine

install-libs: install-libm

install-libm-dirs:
	mkdir -p $(DESTDIR)$(LIBDIR)

install-libm: install-libm-dirs libm.a
	cp -P libm.a $(DESTDIR)$(LIBDIR)
