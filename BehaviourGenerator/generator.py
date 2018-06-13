"""\
---------------------------------------------
USE python <PROGNAME>
OPTIONS:
 -h: print this help message
 -t FILE: read template file
 -r FILE: read source file
 -w FILE: write to file
---------------------------------------------
"""


import sys, getopt, re

class CommandLine:
	def __init__(self):
		opts, args = getopt.getopt(sys.argv[1:],'ht:r:w:')
		opts = dict(opts)

		if len(args) > 0:
			print('please use option tag', file=sys.stderr)
			self.printHelp()

		if '-h' in opts:
			self.printHelp()
			
		if '-t' in opts:
			self.temp_file_name = opts['-t']
		if '-r' in opts:
			self.read_file_name = opts['-r']
		if '-w' in opts:
			self.write_file_name = opts['-w']

	def printHelp(self):
		help = __doc__.replace('<PROGNAME>', sys.argv[0],1)
		print(help, file=sys.stderr)
		sys.exit()


class BehaviourGenerator:

	def __init__(self):
		self.time_line = []
	
	def read_file(self, name):
		self.rfile = open(name, 'r')

	def template_file(self, name):
		self.temp_file = open(name, 'r')

	def write_file(self, name):
		self.wfile = open(name, 'w')

		result = ''
		action_no = 0

		content = self.rfile.readlines()
		
		for line in content:
			parameters = re.split(r'\t+',line.rstrip('\n'))
			try:
				int(parameters[0])
				result = result + 'if(current_action == ' + str(action_no) +  \
					' ) {\n' + \
					'  set_motion('+parameters[2]+');\n' + \
					'  set_colour('+parameters[1]+');\n' + \
					'  if(kilo_ticks > last_state_update + ' + parameters[0] + ') {\n' + \
					'    current_action += 1;\n' + \
					'    last_state_update = kilo_ticks;\n' + \
					'  }\n' + \
					'}\n'
				action_no += 1

			except ValueError:
				if parameters[0] == 'REPEAT':
					result = result + 'for(int i=0;i<'+parameters[1]+';i++){\n' + \
						'current_action = '+str(action_no)+';\n'
				if parameters[0] == 'REPEAT END':
					result = result + '}\n'
		
		print(result)

		result = self.temp_file.read().replace('//INSERT CODE HERE',result,1)
		self.wfile.write(result)

if __name__ == '__main__':

	config = CommandLine()

	bg = BehaviourGenerator()

	bg.template_file(config.temp_file_name)
	bg.read_file(config.read_file_name)
	bg.write_file(config.write_file_name)