# Fuzz Testing

## Instructive to make tests in the SPEL server

### How to connect to the server

In the repo Fuzzy Testing ( https://gitlab.com/spel-uchile/SUCHAI-FS-Fuzzy-Testing.git ) you will find
**jenkinsSpelBack.pem**. To start the server you have to be in the same folder as that file and then execute:

`ssh -i jenkinsSpelBack.pem ubuntu@data.spel.cl`


### Tests in the server
Once the server is on, you can make your own test instead of waiting for the resulst in the pipeline. In order to do that the first step is create the docker, follow the next commands in the server: 

`cd $HOME/suchai-docker`

`git pull`   #to create the docker image with the last configurations

`cd suchai-fs`

`docker build --no-cache -t suchai-fs .`   #command to create the docker image

`docker system prune -f`                    #command to delete and optimized the memory in the server


After that you can run the sh file to make the test fuzzy:

`docker run -v ~/.ssh:/root/.ssh -i suchai-fs bash install_test_fuzzy-framework.sh`

Or you can start the docker in interactive mode so you can run the commands in the .sh one by one or try new commands:

`docker run -t -i suchai-fs /bin/bash`


## Fuzzy Testing in the CI/CD

In order to explain the changes related to this step, here is an explanation of the files add and modified in the repos:

### suchai-docker/suchai-fs

#### Dockerfile

At the end of this file , exactly in the lines 38 to 41 are the instructions to clone the SUCHAI-FS-Fuzzy-Testing. In order to run the experiment it was necessary install the next libraries: `fuzzingbook`, `markdown` and   `pandas`. 

#### install_test_fuzzy-framework.sh

In this file are the instrucctions to execute the test. In here we clone the SUCHAI-Flight-Software and works in the feature/framework branch from the repo. Randomly the program gets a number between 1 and 3 to select the strategy to run in the test. 

The command to run the test is :

`python3 run_experiment.py --iterations a --commands_number b --strategy c`

where a, b and c are numbers. In the .sh this params depends on the strategy selected randomly.

after the execution of the SUCHAI-FS-Fuzzy-Testing  the result is captured in a boolean an then return so the CI/CD can show the result.
 
### suchai-docker/suchai-fs

#### run_experiment.py 

This one is the only file with modifications in the repo.

In the function `run_experiment`  now the function returns a list when each row represents a iteration with the commands list, their params and the exit code which indicates if the command run correctly (1, this means the software accept an incorrect input) or not.  


Now the `main` function returns the result of each iteration receive in the `run_experiment` function.


Finally the main of the script execute the strategy, and receive the result from the execution, then calls the aux functions `print_cmds` to print in a user friendly way the commands send in each iterations and their params and the `check_return_codes function` to return a boolean which indicates if the test was successful or not.


### Pipeline Editor

In this file we create a new stage call `test_fuzz` and then the test call `test_fuzzing` which executes the test when a commit in the "feature/framework" branch of the SUCHAI-Flifht-Software is made.




