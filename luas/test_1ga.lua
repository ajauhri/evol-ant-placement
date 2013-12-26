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
    mutation = 0.1,
    pop = 2,
    generations = 2,
    elitism = 0,
    tournament_size = 1,
    recombination = 0.8,
    exp_weight = 2,
    algorithm = "GA",
    run_simulator = 1,
    max_gain = 1,
    max_coup = 1,
    min_coup = 1,
    auto_seed = 1,
}
