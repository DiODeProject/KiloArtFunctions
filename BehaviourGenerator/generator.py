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

#access information from command line
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

#print help for user
	def printHelp(self):
		help = __doc__.replace('<PROGNAME>', sys.argv[0],1)
		print(help, file=sys.stderr)
		sys.exit()

#generate c files for kilobot behaviour
class BehaviourGenerator:
	
	#read the file that defines behaviour
	def read_file(self, name):
		self.rfile = open(name, 'r')

	#read template file
	def template_file(self, name):
		self.temp_file = open(name, 'r')

	#export to c file
	def write_file(self, name):
		self.wfile = open(name, 'w')

		#initialise variables
		result = ''				#content code
		global_variable = ''	#global variables code
		action_no = 0			#count for actions so that each action only execute once
		nested_action = 0		#count for nested loop, different level shares different action count
		nested_action_no = {}	#dictionary to store different levels of action count in nested loop

		#read file content line by line, store lines as list
		content = self.rfile.readlines()
		
		for line in content:
			#decompose each line to list of parameters
			parameters = re.split(r'\t+',line.rstrip('\n'))


			try:
				#check if the first parameter is an integer
				int(parameters[0])

				#decide different condition for different level of nested loop (or main loop)
				condition = 'if(current_action ==' if nested_action==0 else \
					('if(inner_action' + str(nested_action) + ' ==')
				condition_no = action_no if nested_action ==0 else \
					nested_action_no[nested_action]
				condition_action = 'current_action' if nested_action==0 else \
					'inner_action'+str(nested_action)

				#generate the result (concatenate for each line)
				result = result + condition + str(condition_no) +  \
					' ) {\n' + \
					'  set_motion('+parameters[2]+');\n' + \
					'  set_colour('+parameters[1]+');\n' + \
					'  if(kilo_ticks > last_state_update + ' + parameters[0] + ') {\n' + \
					'    '+condition_action+' += 1;\n' + \
					'    last_state_update = kilo_ticks;\n' + \
					'  }\n' + \
					'}\n'

				#increment the action count (nested looped action count)
				if nested_action == 0:
					action_no += 1
				else:
					nested_action_no[nested_action] += 1

			#if the first parameter is not integer
			except ValueError:

				#if it is the begining of the loop
				if parameters[0] == 'REPEAT':

					#initialise action number
					nested_action_no[nested_action+1] = 0

					#decide different condition for different level of nested loop (or main loop)
					condition = 'if(current_action ==' if nested_action==0 else \
						('if(inner_action' + str(nested_action) + ' ==')
					condition_no = action_no if nested_action ==0 else \
						nested_action_no[nested_action]

					#generate the result (concatenate for each line)
					result = result + condition + str(condition_no) + \
						' ) {\n\n' + \
						'  int inner_action'+str(nested_action+1)+' = 0;\n' + \
						'  int i'+str(nested_action+1)+' = 0;\n' + \
						'  while(i'+str(nested_action+1)+'<'+parameters[1]+'){\n'

					#increment nested count
					nested_action += 1

				#if it is the end of the loop
				if parameters[0] == 'REPEAT END':

					#decide different condition for different level of nested loop (or main loop)
					condition = '    if(inner_action' + str(nested_action) + ' =='
					condition_no = nested_action_no[nested_action]
					condition_action = 'current_action' if nested_action-1==0 else \
						'inner_action'+str(nested_action-1)

					#generate the result (concatenate for each line)
					result = result + condition + str(condition_no) + \
						') {\n' + \
						'      i'+str(nested_action)+'++;\n' + \
						'      inner_action' + str(nested_action) + ' = 0;\n' + \
						'    }\n' + \
						'  }\n\n' + \
						'  '+condition_action+' += 1;\n' + \
						'}\n'

					#decreae the nested count as leaving the loop
					nested_action -= 1

					#increment the action count in outer loop
					if nested_action == 0:
						action_no += 1
					else:
						nested_action_no[nested_action] += 1

		#put the content code into template code
		result = self.temp_file.read().replace('//INSERT CODE HERE',result,1)
		#put the global variables into previous code
		result = result.replace('//INSERT GLOBAL VARIABLES HERE',global_variable,1)

		#print(result)

		#export to the file
		self.wfile.write(result)

if __name__ == '__main__':

	config = CommandLine()

	bg = BehaviourGenerator()

	bg.template_file(config.temp_file_name)
	bg.read_file(config.read_file_name)
	bg.write_file(config.write_file_name)