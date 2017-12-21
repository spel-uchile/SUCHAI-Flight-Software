hooks
exportHTML

	"The following code shows dependencies between "
	| root p b allSystemModules allDrivers relevantModules lb |
	root := '../SUCHAI-Flight-Software'.
	p := CPPProcessor new.
	p processRootFolder: root asFileReference.
	p resolveDependencies.
	
	allSystemModules := p modules select: [ :m | '*/system*' match: m filename ].
	allDrivers := p modules select: [ :m | '*/drivers/Linux/*' match: m filename ].
	relevantModules := allSystemModules, allDrivers.
	relevantModules := relevantModules reject: [ :m | m filename = '*/libcsp/*' ].
	
	b := RTMondrian new.
	b shape rectangle 
		color: Color darkGray;
		if: [ :f | '*/repo*' match: f filename ] color: Color red;
		if: [ :f | '*/cmd*' match: f filename ] color: Color blue;
		if: [ :f | '*/task*' match: f filename ] color: Color green;
		if: [ :f | '*/drivers*' match: f filename ] color: Color lightGray;
		
		if: [ :f | 
			('*/task*' match: f filename) and: [ f dependOn: '*/cmd*' ] ] borderColor: Color red.
	b interaction highlightNeighbour:  #dependentModules color: Color orange.
"	b shape box size: #loc."
	b nodes: relevantModules.
	
	
	b shape arrowedLine 
		withOffset;
		color: (Color blue alpha: 0.2);
		width: 2.
	b edges 
		moveBehind;
		connectToAll: #dependentModules.
		
	b layout sugiyama
		verticalGap: 20;
		horizontalGap: 10.
	
	b normalizer normalizeSize: #loc.
	b build.
	
	lb := RTLegendBuilder new.
	lb setAsFixed.
	lb view: b view.
	lb addText: 'Dependencies between modules'.
	lb addColor: Color green text: 'Task'.
	lb addColor: Color red text: 'Repos'.
	lb addColor: Color blue text: 'Commands'.
	lb addColor: Color lightGray text: 'Drivers'.
	lb build.
	
	RTHTML5Exporter new export: b view