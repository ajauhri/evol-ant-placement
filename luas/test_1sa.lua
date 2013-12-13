load_platform("S1 - 4ft square plate fixed.nec")
add_antenna("A2 - 150mono fixed.nec")
add_point( -0.406400,  -0.101600,   0.000000)
add_point( -0.304800,  -0.101600,   0.000000)
add_point( -0.203200,  -0.101600,   0.000000)
add_point( -0.101600,  -0.101600,   0.000000)
add_point(  0.000000,  -0.101600,   0.000000)
add_point( -0.304800,  -0.203200,   0.000000)
add_point( -0.304800,  -0.304800,   0.000000)
add_point( -0.203200,  -0.304800,   0.000000)
add_point( -0.101600,  -0.304800,   0.000000)
add_point( -0.101600,  -0.203200,   0.000000)
add_point( -0.203200,  -0.203200,   0.000000)
add_point(  0.000000,  -0.203200,   0.000000)
add_point(  0.000000,  -0.304800,   0.000000)
add_point(  0.000000,  -0.406400,   0.000000)
add_point( -0.101600,  -0.406400,   0.000000)
add_point( -0.203200,  -0.406400,   0.000000)
add_point( -0.304800,  -0.406400,   0.000000)
add_antenna("A5 - 300dipole fixed.nec")
add_point( -0.304800,   0.203200,   0.000000)
add_point( -0.203200,   0.203200,   0.000000)
add_point( -0.304800,   0.101600,   0.000000)
add_point( -0.203200,   0.101600,   0.000000)
add_point( -0.101600,   0.101600,   0.000000)
add_point(  0.000000,   0.101600,   0.000000)
add_point(  0.000000,   0.203200,   0.000000)
add_point( -0.101600,   0.203200,   0.000000)
add_point( -0.101600,   0.304800,   0.000000)
add_point( -0.101600,   0.406400,   0.000000)
add_point( -0.203200,   0.406400,   0.000000)
add_point( -0.304800,   0.406400,   0.000000)
add_point( -0.304800,   0.304800,   0.000000)
add_point(  0.000000,   0.304800,   0.000000)
add_point(  0.000000,   0.406400,   0.000000)
add_point( -0.203200,   0.304800,   0.000000)
params = {
    iterations=20,
    mutation=0.1,
    init_temp=25,
    temp_factor=1.92,
    iterations_per_temp_change=1000,
    output_freq=5,
    convergence_factor=1,
    gen_condition=10,
    fitness_condition=1,
    exp_weight=2,
    auto_seed=1,
    seed=30
}
