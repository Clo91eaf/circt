// RUN: circt-opt %s -split-input-file -verify-diagnostics

// 'const' firrtl.reg is invalid
firrtl.circuit "test" {
firrtl.module @test(in %clock: !firrtl.clock) {
  // expected-error @+1 {{'firrtl.reg' op result #0 must be a passive non-'const' base type that does not contain analog, but got '!firrtl.const.uint<1>'}}
  %r = firrtl.reg %clock : !firrtl.clock, !firrtl.const.uint<1>
}
}

// -----

// 'const' firrtl.regreset is invalid
firrtl.circuit "test" {
firrtl.module @test(in %clock: !firrtl.clock, in %reset: !firrtl.asyncreset, in %resetVal: !firrtl.const.uint<1>) {
  // expected-error @+1 {{'firrtl.regreset' op result #0 must be a passive non-'const' base type that does not contain analog, but got '!firrtl.const.uint<1>'}}
  %r = firrtl.regreset %clock, %reset, %resetVal : !firrtl.clock, !firrtl.asyncreset, !firrtl.const.uint<1>, !firrtl.const.uint<1>
}
}

// -----

// firrtl.strictconnect non-'const' to 'const' flow is invalid
firrtl.circuit "test" {
firrtl.module @test(in %in : !firrtl.bundle<a: uint<1>, b: sint<2>>, out %out : !firrtl.bundle<a: const.uint<1>, b: sint<2>>) {
  // expected-error @+1 {{type mismatch}}
  firrtl.strictconnect %out, %in : !firrtl.bundle<a: const.uint<1>, b: sint<2>>, !firrtl.bundle<a: uint<1>, b: sint<2>>
}
}

// -----

// Primitive ops with all 'const' operands must have a 'const' result type
firrtl.circuit "test" {
firrtl.module @test(in %a: !firrtl.const.uint<4>, in %b: !firrtl.const.uint<4>) {
  // expected-error @+1 {{'firrtl.and' op inferred type(s) '!firrtl.const.uint<4>' are incompatible with return type(s) of operation '!firrtl.uint<4>'}}
  %0 = firrtl.and %a, %b : (!firrtl.const.uint<4>, !firrtl.const.uint<4>) -> !firrtl.uint<4>
}
}

// -----

// Primitive ops with mixed 'const' operands must have a non-'const' result type
firrtl.circuit "test" {
firrtl.module @test(in %a: !firrtl.const.uint<4>, in %b: !firrtl.uint<4>) {
  // expected-error @+1 {{'firrtl.and' op inferred type(s) '!firrtl.uint<4>' are incompatible with return type(s) of operation '!firrtl.const.uint<4>'}}
  %0 = firrtl.and %a, %b : (!firrtl.const.uint<4>, !firrtl.uint<4>) -> !firrtl.const.uint<4>
}
}
