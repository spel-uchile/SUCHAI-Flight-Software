# Dependency Graph

### Introduction
This Guide assumes that you already clone the SUCHAI-Flight-Software git repository and that you are executing all the following commands from the folder in which this document reside.

### Installation.
This document will guide you to download and use a working "Pharo Image" with all the necessary software already installed.

#### Linux Installation (Ubuntu 16.04)
Download the Pharo Image with the following command

```bash
wget https://data.spel.cl/fs-pharo-image.tar.xz
tar xf fs-pharo-image.tar.xz
cd fs-pharo-image
```

In the image directory you can run a Pharo *.st script in the terminal

```bash
'./pharo-vm/pharo' -vm-display-null Pharo.image export_code_html.st
'./pharo-vm/pharo' -vm-display-null Pharo.image export_code_svg.st
```

#### Mac Installation
Download the Pharo Image with the following command

```bash
wget https://data.spel.cl/fs-pharo-image.tar.xz
wget http://data.spel.cl/MAC61-x64.zip
tar xf fs-pharo-image.tar.xz
unzip MAC61-x64.zip
cd fs-pharo-image
```

In the image directory you can run a Pharo *.st script in the terminal using MAC 

```bash
./Pharo.app/Contents/MacOS/Pharo -vm-display-null Pharo.image export_code_html.st
./Pharo.app/Contents/MacOS/Pharo -vm-display-null Pharo.image export_code_svg.st
```

The first command will produce an index.html file with the dependency graph of the code in the path provided inside the script. The second command will produce the same graph but in SVG format.

### Coding Visualizations.
To export the dependency graph visualization a class called SuchaiAnalisis written in Pharo is used. The class uses Roassal library to visualize the code. This section will explain how to code the method buildVisualization used in the class.

Open Pharo image

```bash
# Linux executable
./pharo-vm/pharo Pharo.image

# MAC executable
./Pharo.app/Contents/MacOS/Pharo Pharo.image
```

Then open a Playground screen and run the next smalltalk code

```smalltalk
s := SuchaiAnalysis new.
s buildViewFromPath: '../../../../SUCHAI-Flight-Software/src'.
```

This will create a Roassal Visualization from the code in src. Now let us code it. Delete the previous code or open a clean Playground.

First we have to create an instance of CPPProcessor and give it a path.

```smalltalk
"Create CPPProcessor with a custom root directory"
root := '../../../../SUCHAI-Flight-Software/src'.
p := CPPProcessor new.
"Add blacklist wildcards for unwanted folders"
p addBlackListRule: '*drivers/nanomind/*'.
p addBlackListRule: '*os/FreeRTOS*'.
p processRootFolder: root asFileReference.
p resolveDependencies.
```

Next we will separate modules in driver, os and system and reject some third party libraries.

```smalltalk
"Separate modules in system, drivers and os"
allSystemModules := p modules
    select: [ :m | '*/system*' match: m filename ].
allDrivers := p modules
    select: [ :m | '*/drivers/Linux/*' match: m filename ].
allOs := p modules select: [ :m | '*/os/Linux/*' match: m filename ].

"Create relevantModules list"
relevantModules := allSystemModules , allDrivers , allOs.
relevantModules := relevantModules
    reject: [ :m | '*/util*' match: m filename ].
relevantModules := relevantModules
    reject: [ :m | '*/globals*' match: m filename ].
relevantModules := relevantModules
    reject: [ :m | '*pthread_queue*' match: m filename ].
```

Let us create a nice color palette for our plot.

```smalltalk
"Create color palette"
color1 := Color r: 0.651 g: 0.808 b: 0.89.
color2 := Color r: 0.022 g: 0.471 b: 0.706.
color3 := Color r: 0.698 g: 0.875 b: 0.541.
color4 := Color r: 0.2 g: 0.627 b: 0.173.
color5 := Color r: 0.984 g: 0.604 b: 0.6.
color6 := Color r: 0.89 g: 0.102 b: 0.11.
color7 := Color r: 0.992 g: 0.749 b: 0.435.
```

For the next step we will assign the color palette to different modules and create interaction between blocks in the plot.

```smalltalk
"Create a RTMondrian instance"
b := RTMondrian new.
b interaction popupText: #suchaiDisplayName.

"Assign color palette"
b shape rectangle
    color: Color darkGray;
    width: [ :cls | (cls dependentModules size + 1) * 20 * 0.6 ];
    height: [ :cls | cls loc * 0.6 ];
    if: [ :f | '*/repo*' match: f filename ] color: color3;
    if: [ :f | '*/cmd*' match: f filename ] color: color4;
    if: [ :f | '*/task*' match: f filename ] color: color5;
    if: [ :f | '*/drivers*' match: f filename ] color: color1;
    if: [ :f | '*/main*' match: f filename ] color: color2;
    if: [ :f | '*Dispatcher*' match: f filename ] color: color7;
    if: [ :f | '*Executer*' match: f filename ] color: color6;
    if: [ :f | ('*/task*' match: f filename) and: [ f dependOn: '*/cmd*' ] ]
        borderColor: Color red.
        
"Create interactions"
b interaction showEdge
    connectToAll: #dependentModules;
    shape: (RTLine new color: Color orange).
b interaction
    highlightNeighbors: #dependentModules
    color: Color orange.
b nodes: relevantModules.
b shape arrowedLine
    headOffset: 0.8;
    withShorterDistanceAttachPointWithJump;
    color: (Color black alpha: 0.7);
    width: 2.
b edges
    moveBehind;
    connectToAll: #dependentModules.
b layout sugiyama
    verticalGap: 50;
    horizontalGap: 25.
b build.
```

Finally lets create a legend and build it

```smalltalk
"Create legend"
lb := RTLegendBuilder new.
lb setAsFixed.
lb view: b view.
lb addText: 'Modules Types'.
lb addColor: color5 text: 'Clients'.
lb addColor: color3 text: 'Repositories'.
lb addColor: color4 text: 'Commands'.
lb addColor: color1 text: 'Drivers'.
lb addColor: color7 text: 'Invoker'.
lb addColor: color6 text: 'Receiver'.
lb addColor: Color darkGray text: 'Operating System'.
lb addColor: color2 text: 'main'.
lb addRectanglePolymetricWidth: 'number of dependencies'
    height: 'lines of code'
    box: 'Module'.
lb build
```

Run the code, the Dependency Graph Visualization should be plot next to Playground screen.

### Python helper function.

For easy access to different commits a python file with helper functions have been created. The following execution shows how to create an html file from an specific path and commit.

```bash
python3 suchai_analysis.py ../../../../SUCHAI-Flight-Software/src --hash d6496a2
```

Other functions like plotting in svg or selecting a range of commit to plot can also be performed. For helping of how to use these features run

```bash
python3 suchai_analysis.py -h
```





