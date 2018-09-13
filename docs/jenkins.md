#Jenkins settings

###Jenkins server connection via ssh
To establish a Jenkins server connection via ssh is necessary to 
have the "jenkinsSpel.pem" file. Use the command line to go into 
the folder where this file is located in, then type the following 
command:

```bash
ssh -i "jenkinsSpel.pem" ubuntu@jenkins.spel.cl
```

####Connecting as jenkins user
Once connected, it is possible to change the ubuntu user to
jenkins user from the command line:

```bash
su - jenkins
```
The password required below is **1jenkins2**. This is the sudo
password for the jenkins user too.

####Access to the Jenkins server in browser
jenkins.spel.cl

###Creating a user
An existing user must create the account of any new Jenkins user
that needs to have entire access to the projects properties.

###Editing SUCHAI-Flight-Software-pipeline pipeline
Click on the project, then go the "Configure" section. You will be 
able to edit the script in the "Pipeline" section. Each stage inside
the script represents a specific process. The format to add 
a new stage is:

_stage ('New stage') {_\
_//do something_ \
_}_

The format to add a new stage with a shell script inside is:

_stage ('New stage') {_\
_sh """_ \
_#some commands here_ \
_"""_ \
_}_


