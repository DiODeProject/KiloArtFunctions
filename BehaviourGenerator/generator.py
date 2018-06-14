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
		global_variable = ''
		action_no = 0
		nested_action = 0
		nested_action_no = {}

		content = self.rfile.readlines()
		
		for line in content:
			parameters = re.split(r'\t+',line.rstrip('\n'))
			try:
				int(parameters[0])

				condition = 'if(current_action ==' if nested_action==0 else \
					('if(inner_action' + str(nested_action) + ' ==')
				condition_no = action_no if nested_action ==0 else \
					nested_action_no[nested_action]
				condition_action = 'current_action' if nested_action==0 else \
					'inner_action'+str(nested_action)

				result = result + condition + str(condition_no) +  \
					' ) {\n' + \
					'  set_motion('+parameters[2]+');\n' + \
					'  set_colour('+parameters[1]+');\n' + \
					'  if(kilo_ticks > last_state_update + ' + parameters[0] + ') {\n' + \
					'    '+condition_action+' += 1;\n' + \
					'    last_state_update = kilo_ticks;\n' + \
					'  }\n' + \
					'}\n'

				if nested_action == 0:
					action_no += 1
				else:
					nested_action_no[nested_action] += 1

			except ValueError:
				if parameters[0] == 'REPEAT':

					nested_action_no[nested_action+1] = 0
					condition = 'if(current_action ==' if nested_action==0 else \
						('if(inner_action' + str(nested_action) + ' ==')
					condition_no = action_no if nested_action ==0 else \
						nested_action_no[nested_action]

					result = result + condition + str(condition_no) + \
						' ) {\n\n' + \
						'  int i'+str(nested_action+1)+' = 0;\n' + \
						'  while(i'+str(nested_action+1)+'<'+parameters[1]+'){\n'
					global_variable = global_variable + 'int inner_action'+str(nested_action+1)+' = 0;\n'
					nested_action += 1
				if parameters[0] == 'REPEAT END':

					condition = '    if(inner_action' + str(nested_action) + ' =='
					condition_no = nested_action_no[nested_action]
					condition_action = 'current_action' if nested_action-1==0 else \
						'inner_action'+str(nested_action-1)

					result = result + condition + str(condition_no) + \
						') {\n' + \
						'      i'+str(nested_action)+'++;\n' + \
						'      inner_action' + str(nested_action) + ' = 0;\n' + \
						'    }\n' + \
						'  }\n\n' + \
						'  '+condition_action+' += 1;\n' + \
						'}\n'

					nested_action -= 1
					if nested_action == 0:
						action_no += 1
					else:
						nested_action_no[nested_action] += 1

		result = self.temp_file.read().replace('//INSERT CODE HERE',result,1)
		result = result.replace('//INSERT GLOBAL VARIABLES HERE',global_variable,1)

		print(result)

		self.wfile.write(result)

if __name__ == '__main__':

	config = CommandLine()

	bg = BehaviourGenerator()

	bg.template_file(config.temp_file_name)
	bg.read_file(config.read_file_name)
	bg.write_file(config.write_file_name)