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
	pop = 10,
	generations = 5,
	elitism = 2,
	tournament_size = 5,
	mutation = 0.4,
	recombination = 0.2,
	exp_weight = 2,
	local_run = 0,
	algorithm = "GA", 
    auto_seed = 1,
    seed = 30
}