from flask import Flask, url_for, render_template, jsonify, request
import os, ctypes

fnest = Flask(__name__)

# Function to easily find your assets
# In your template use <link rel=stylesheet href="{{ 
#static('filename') }}">
fnest.jinja_env.globals['static'] = (
    lambda filename: url_for('static', filename = filename)
)

fnest.config['current_temp'] = 66
fnest.config['set_temp'] = 70


@fnest.route('/setTemp', methods=['GET'])
def set():
    lib = ctypes.cdll.LoadLibrary('./mmap.so')
    set_temp = int(request.args['setpoint'])
    fnest.config['set_temp'] = set_temp
    lib.change_set_temp(set_temp)
    cur_temp = lib.get_cur_temp() 
    return jsonify(set_temp=set_temp, cur_temp = cur_temp)


@fnest.route('/getTemp', methods=['GET'])
def get():
    lib = ctypes.cdll.LoadLibrary('./mmap.so')
    fnest.config['current_temp'] = lib.get_cur_temp();
    fnest.config['set_temp'] = lib.get_set_temp();
    return jsonify(
        cur_temp = fnest.config['current_temp'], 
        set_temp = fnest.config['set_temp'])

@fnest.route('/')
def index():
    return "hello world"


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 3933))
    fnest.run(host='0.0.0.0', port=port)

