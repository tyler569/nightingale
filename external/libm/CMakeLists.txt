set(LIBM_SOURCES
    arch/x86_64/e_acos.S
    arch/x86_64/e_asin.S
    arch/x86_64/e_atan2.S
    arch/x86_64/e_expf.S
    arch/x86_64/e_exp.S
    arch/x86_64/e_fmod.S
    arch/x86_64/e_log10f.S
    arch/x86_64/e_log10.S
    arch/x86_64/e_log2f.S
    arch/x86_64/e_log2.S
    arch/x86_64/e_logf.S
    arch/x86_64/e_log.S
    arch/x86_64/e_remainderf.S
    arch/x86_64/e_remainder.S
    arch/x86_64/e_scalbf.S
    arch/x86_64/e_scalb.S
    arch/x86_64/e_sqrtf.S
    arch/x86_64/e_sqrt.S
    arch/x86_64/fabs.S
    arch/x86_64/fenv.c
    arch/x86_64/flt_rounds.S
    arch/x86_64/fpgetmask.S
    arch/x86_64/fpgetprec.S
    arch/x86_64/fpgetround.S
    arch/x86_64/fpgetsticky.S
    arch/x86_64/fpsetmask.S
    arch/x86_64/fpsetprec.S
    arch/x86_64/fpsetround.S
    arch/x86_64/fpsetsticky.S
    arch/x86_64/lrint.S
    arch/x86_64/s_atanf.S
    arch/x86_64/s_atan.S
    arch/x86_64/s_ceilf.S
    arch/x86_64/s_ceil.S
    arch/x86_64/s_copysignf.S
    arch/x86_64/s_copysign.S
    arch/x86_64/s_cosf.S
    arch/x86_64/s_cos.S
    arch/x86_64/s_finitef.S
    arch/x86_64/s_finite.S
    arch/x86_64/s_floorf.S
    arch/x86_64/s_floor.S
    arch/x86_64/s_ilogbf.S
    arch/x86_64/s_ilogbl.S
    arch/x86_64/s_ilogb.S
    arch/x86_64/s_log1pf.S
    arch/x86_64/s_log1p.S
    arch/x86_64/s_logbf.S
    arch/x86_64/s_logbl.S
    arch/x86_64/s_logb.S
    arch/x86_64/s_rintf.S
    arch/x86_64/s_rint.S
    arch/x86_64/s_scalbnf.S
    arch/x86_64/s_scalbn.S
    arch/x86_64/s_significandf.S
    arch/x86_64/s_significand.S
    arch/x86_64/s_sinf.S
    arch/x86_64/s_sin.S
    arch/x86_64/s_tanf.S
    arch/x86_64/s_tan.S

    src/b_exp.c
    src/b_log.c
    src/b_tgamma.c
    src/compat_frexp_ieee754.c
    src/compat_ldexp_ieee754.c
    src/e_acos.c
    src/e_acosf.c
    src/e_acosh.c
    src/e_acoshf.c
    src/e_asin.c
    src/e_asinf.c
    src/e_atan2.c
    src/e_atan2f.c
    src/e_atanh.c
    src/e_atanhf.c
    src/e_cosh.c
    src/e_coshf.c
    src/e_exp.c
    src/e_expf.c
    src/e_fmod.c
    src/e_fmodf.c
    src/e_hypot.c
    src/e_hypotf.c
    src/e_j0.c
    src/e_j0f.c
    src/e_j1.c
    src/e_j1f.c
    src/e_jn.c
    src/e_jnf.c
    src/e_lgammaf_r.c
    src/e_lgamma_r.c
    src/e_log10.c
    src/e_log10f.c
    src/e_log2.c
    src/e_log2f.c
    src/e_log.c
    src/e_logf.c
    src/e_pow.c
    src/e_powf.c
    src/e_remainder.c
    src/e_remainderf.c
    src/e_rem_pio2.c
    src/e_rem_pio2f.c
    src/e_scalb.c
    src/e_scalbf.c
    src/e_sinh.c
    src/e_sinhf.c
    src/e_sqrt.c
    src/e_sqrtf.c
    src/fpclassifyd_ieee754.c
    src/fpclassifyf_ieee754.c
    src/fpclassifyl.c
    src/fpclassifyl_ieee754.c
    src/isfinited_ieee754.c
    src/isfinitef_ieee754.c
    src/isfinitel.c
    src/isfinitel_ieee754.c
    src/isinfd_ieee754.c
    src/isinff_ieee754.c
    src/isinfl.c
    src/isinfl_ieee754.c
    src/isnand_ieee754.c
    src/isnanf_ieee754.c
    src/isnanl.c
    src/isnanl_ieee754.c
    src/k_cos.c
    src/k_cosf.c
    src/k_rem_pio2.c
    src/k_rem_pio2f.c
    src/k_sin.c
    src/k_sinf.c
    src/k_standard.c
    src/k_tan.c
    src/k_tanf.c
    src/llrint.c
    src/llrintf.c
    src/llround.c
    src/llroundf.c
    src/lrint.c
    src/lrintf.c
    src/lround.c
    src/lroundf.c
    src/modf_ieee754.c
    src/nan.c
    src/nanf.c
    src/nanl.c
    src/s_asinh.c
    src/s_asinhf.c
    src/s_atan.c
    src/s_atanf.c
    src/s_cbrt.c
    src/s_cbrtf.c
    src/s_ceil.c
    src/s_ceilf.c
    src/s_copysign.c
    src/s_copysignf.c
    src/s_copysignl.c
    src/s_cos.c
    src/s_cosf.c
    src/s_erf.c
    src/s_erff.c
    src/s_exp2.c
    src/s_exp2f.c
    src/s_expm1.c
    src/s_expm1f.c
    src/s_fabsf.c
    src/s_fabsl.c
    src/s_fdim.c
    src/s_finite.c
    src/s_finitef.c
    src/s_floor.c
    src/s_floorf.c
    src/s_fmax.c
    src/s_fmaxf.c
    src/s_fmaxl.c
    src/s_fmin.c
    src/s_fminf.c
    src/s_fminl.c
    src/s_frexp.c
    src/s_frexpf.c
    src/signbitd_ieee754.c
    src/signbitf_ieee754.c
    src/signbitl.c
    src/s_ilogb.c
    src/s_ilogbf.c
    src/s_ilogbl.c
    src/s_isinff.c
    src/s_isnanf.c
    src/s_ldexp.c
    src/s_ldexpf.c
    src/s_lib_version.c
    src/s_log1p.c
    src/s_log1pf.c
    src/s_logb.c
    src/s_logbf.c
    src/s_logbl.c
    src/s_matherr.c
    src/s_modf.c
    src/s_modff.c
    src/s_nextafter.c
    src/s_nextafterf.c
    src/s_nextafterl.c
    src/s_nexttoward.c
    src/s_remquo.c
    src/s_remquof.c
    src/s_rint.c
    src/s_rintf.c
    src/s_round.c
    src/s_roundf.c
    src/s_scalbn.c
    src/s_scalbnf.c
    src/s_scalbnl.c
    src/s_signgam.c
    src/s_significand.c
    src/s_significandf.c
    src/s_sin.c
    src/s_sinf.c
    src/s_tan.c
    src/s_tanf.c
    src/s_tanh.c
    src/s_tanhf.c
    src/s_tgammaf.c
    src/s_trunc.c
    src/s_truncf.c
    src/w_acos.c
    src/w_acosf.c
    src/w_acosh.c
    src/w_acoshf.c
    src/w_asin.c
    src/w_asinf.c
    src/w_atan2.c
    src/w_atan2f.c
    src/w_atanh.c
    src/w_atanhf.c
    src/w_cosh.c
    src/w_coshf.c
    src/w_drem.c
    src/w_dremf.c
    src/w_exp.c
    src/w_expf.c
    src/w_fmod.c
    src/w_fmodf.c
    src/w_gamma.c
    src/w_gammaf.c
    src/w_gammaf_r.c
    src/w_gamma_r.c
    src/w_hypot.c
    src/w_hypotf.c
    src/w_j0.c
    src/w_j0f.c
    src/w_j1.c
    src/w_j1f.c
    src/w_jn.c
    src/w_jnf.c
    src/w_lgamma.c
    src/w_lgammaf.c
    src/w_lgammaf_r.c
    src/w_lgamma_r.c
    src/w_log10.c
    src/w_log10f.c
    src/w_log2.c
    src/w_log2f.c
    src/w_log.c
    src/w_logf.c
    src/w_pow.c
    src/w_powf.c
    src/w_remainder.c
    src/w_remainderf.c
    src/w_scalb.c
    src/w_scalbf.c
    src/w_sinh.c
    src/w_sinhf.c
    src/w_sqrt.c
    src/w_sqrtf.c

    complex/cabs.c
    complex/cabsf.c
    complex/cacos.c
    complex/cacosf.c
    complex/cacosh.c
    complex/cacoshf.c
    complex/carg.c
    complex/cargf.c
    complex/casin.c
    complex/casinf.c
    complex/casinh.c
    complex/casinhf.c
    complex/catan.c
    complex/catanf.c
    complex/catanh.c
    complex/catanhf.c
    complex/ccos.c
    complex/ccosf.c
    complex/ccosh.c
    complex/ccoshf.c
    complex/cephes_subr.c
    complex/cephes_subrf.c
    complex/cexp.c
    complex/cexpf.c
    complex/cimag.c
    complex/cimagf.c
    complex/cimagl.c
    complex/clog.c
    complex/clogf.c
    complex/conj.c
    complex/conjf.c
    complex/conjl.c
    complex/cpow.c
    complex/cpowf.c
    complex/cproj.c
    complex/cprojf.c
    complex/cprojl.c
    complex/creal.c
    complex/crealf.c
    complex/creall.c
    complex/csin.c
    complex/csinf.c
    complex/csinh.c
    complex/csinhf.c
    complex/csqrt.c
    complex/csqrtf.c
    complex/ctan.c
    complex/ctanf.c
    complex/ctanh.c
    complex/ctanhf.c
)

add_compile_definitions(_MULTI_LIBM=1 _POSIX_MODE=1 __USE_SORTIX=1)

add_compile_options(
    -Wno-misleading-indentation
    -Wno-shift-negative-value
    -Wno-pedantic
)

include_directories(BEFORE include)
include_directories(BEFORE src)
include_directories(BEFORE arch/x86_64)

add_library(m STATIC ${LIBM_SOURCES})
add_dependencies(m c)
install(TARGETS m LIBRARY)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX}/usr)
