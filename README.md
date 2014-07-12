evol-ant-placement 
====

Dependendcies
----

sudo apt-get install liblua5.2-dev 

sudo apt-get install libboost-all-dev

sudo apt-get install automake libtool

sudo apt-get install necpp
(http://elec.otago.ac.nz/w/index.php/Necpp)

sudo apt-get install parallel


Compiling & Running 
----
Run `make` to create executable called `evol_ant`

Run executable. For instance - `evol_ant -i luas/tc1_ga.lua -r 1`

Input NEC (\*.nec) and output (\*.out) files are stored under `runs` folder. Best hypothesis for each run are stored under the `results` folder. 
