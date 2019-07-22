# CSO17-Labs
This repository contains cso labs skeleton code. You need to sync from the upstream (described below) before each lab to obtain the lates code. You will also use this repository to submit your code. The structure of this code submission is the same as recitation.

### Programming Environment
You are required to do all recitation exercises and labs on the class virtual machine (based on Ubuntu Linux). To get the virtual machine running on your personal desktop or laptop, take the following steps (note that recitation exercises and labs share the same programming environment, you only have to do this once).

1. Download the [Virtualbox Version 5.1.26](https://www.virtualbox.org/wiki/Downloads) (Please just use version 5.1.26 and not any other version.) Choose the right binary to download according to the type of operating system running on your laptop.
2. Download the class virtual machine image [here](http://news.cs.nyu.edu/~fegin/nyu-cso-17fall/ubuntu16.04.ova). This file is fairly large (>2GB), so you need to be patient.
3. Install and launch Virtualbox. On the Virtualbox application toolbar, under the Menu item "File", click on "Import appliance...", and choose the previously downloaded ubuntu16.04.ova file when prompted.
4. After importing, start the virtual machine named "cso17-VirtualBox" and log in using the username and password (given in Piazza). Change your password.

### Labs Repository Initialization
First of all, please create a github account and accept the lab invitation given in Piazza. Then take the following steps in the the class virtual machine.

```
$ cd /home/cso17/cso
$ git clone https://github.com/nyu-cso-17fall/labs-YOUR_GITHUB_USER_NAME
  # For example, my user name is fegin and the command is like the next line.
  # git clone https://github.com/nyu-cso-17fall/labs-fegin
$ cd labs-YOUR_GITHUB_USER_NAME
$ git remote add upstream https://github.com/nyu-cso-17fall/cso17-labs
$ git pull upstream master
$ git push
$ git config --global user.email "YOUR_EMAIL_ON_GITHUB"
$ git config --global user.name "YOUR_ID_ON_GITHUB"
```

### Sync 
Please sync before doing each lab.
```bash
$ git pull upstream master
$ git push
```

### Submit Solutions
When you finish the exercises, please submit the files (will be specified on the lab page).
```
$ git add FILES_YOU_NEED_TO_SUBMIT
$ git commit -m "some comments"
$ git push
```
