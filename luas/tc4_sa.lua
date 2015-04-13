load_platform("input/S1 - 4ft square plate fixed.nec")
add_antenna("input/A1 - 75mono fixed.nec")
add_point( -0.609600,  -0.406400,   0.000000)
add_point( -0.609600,  -0.508000,   0.000000)
add_point( -0.609600,  -0.609600,   0.000000)
add_point( -0.508000,  -0.609600,   0.000000)
add_point( -0.508000,  -0.508000,   0.000000)
add_point( -0.508000,  -0.406400,   0.000000)
add_point( -0.406400,  -0.406400,   0.000000)
add_point( -0.406400,  -0.508000,   0.000000)
add_point( -0.406400,  -0.609600,   0.000000)
add_point( -0.304800,  -0.609600,   0.000000)
add_point( -0.304800,  -0.508000,   0.000000)
add_point( -0.304800,  -0.406400,   0.000000)
add_antenna("input/A2 - 150mono fixed.nec")
add_point( -0.609600,   0.609600,   0.000000)
add_point( -0.609600,   0.508000,   0.000000)
add_point( -0.609600,   0.406400,   0.000000)
add_point( -0.508000,   0.406400,   0.000000)
add_point( -0.508000,   0.508000,   0.000000)
add_point( -0.508000,   0.609600,   0.000000)
add_point( -0.406400,   0.609600,   0.000000)
add_point( -0.406400,   0.508000,   0.000000)
add_point( -0.406400,   0.406400,   0.000000)
add_point( -0.304800,   0.406400,   0.000000)
add_point( -0.304800,   0.508000,   0.000000)
add_point( -0.304800,   0.609600,   0.000000)
add_antenna("input/A3 - 150dipole fixed.nec")
add_point(  0.304800,   0.609600,   0.000000)
add_point(  0.304800,   0.508000,   0.000000)
add_point(  0.304800,   0.406400,   0.000000)
add_point(  0.406400,   0.406400,   0.000000)
add_point(  0.508000,   0.406400,   0.000000)
add_point(  0.609600,   0.406400,   0.000000)
add_point(  0.609600,   0.508000,   0.000000)
add_point(  0.609600,   0.609600,   0.000000)
add_point(  0.508000,   0.609600,   0.000000)
add_point(  0.508000,   0.508000,   0.000000)
add_point(  0.406400,   0.609600,   0.000000)
add_point(  0.406400,   0.508000,   0.000000)
add_antenna("input/A5 - 300dipole fixed.nec")
add_point(  0.203200,  -0.406400,   0.000000)
add_point(  0.203200,  -0.508000,   0.000000)
add_point(  0.203200,  -0.609600,   0.000000)
add_point(  0.304800,  -0.609600,   0.000000)
add_point(  0.304800,  -0.508000,   0.000000)
add_point(  0.304800,  -0.406400,   0.000000)
add_point(  0.406400,  -0.406400,   0.000000)
add_point(  0.508000,  -0.406400,   0.000000)
add_point(  0.508000,  -0.508000,   0.000000)
add_point(  0.406400,  -0.508000,   0.000000)
add_point(  0.406400,  -0.609600,   0.000000)
add_point(  0.508000,  -0.609600,   0.000000)
params = {
    iterations = 10400,
    cooling_factor = 0.995,
    convergence_factor = 0.1,
    mutation = 0.1,
    exp_weight = 2,
    algorithm = "SA",
    run_simulator = 1,
    max_gain = 7.21229e+08,
    max_coup = 28.464,
    min_coup = -163.098,
    temp_pop_factor = 0.1,
    auto_seed = 1,
    temp_error = 0.05,
    accept_prob = 0.9
}
