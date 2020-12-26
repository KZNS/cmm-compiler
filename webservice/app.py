from flask import Flask, render_template, jsonify, request
import os
import subprocess
from PIL import Image
import base64
from io import BytesIO
import webbrowser
# pipreqs . --encoding=utf8 --force
app = Flask(__name__)
logging_levels = []

print('123123')
def build_compiler():  # build the compiler first be there no existing built version
    cwd = os.getcwd()
    # cd "d:\github\cmm_compiler\compiler\" ; if ($?) { g++ compiler.cpp -o ../webservice/compiler } ; if ($?) { .\compiler }
    os.chdir('../compiler')
    cmd = """g++ compiler.cpp -o ../webservice/compiler 2>&1"""
    """
    if successful, there should be no errors, so we are redirectin
    g errors to stdout, and if outputs is not '', we have an error.
    """
    rs = os.popen(cmd)
    res = rs.read()
    if res != '':
        print('!!! FATAL: compiler compile error !!!')
        print('-'*10)
        print(res)
        exit(0)
    os.chdir(cwd)
    print('compiler compile successful, starting web server.')


def pil_base64(image):
    img_buffer = BytesIO()
    image.save(img_buffer, format="jpeg")
    byte_data = img_buffer.getvalue()
    base64_str = base64.b64encode(byte_data)
    return base64_str


@app.route('/', methods=["GET"])
def main():
    return render_template("index.html")


@app.route('/', methods=["POST"])
def comp():
    r = request.get_json()
    logging = r.get('logging') == 'on'
    if logging:
        logging_level = r.get('loggingLevel')
        if logging_level not in logging_levels:
            logging_level = 'info'
    content = r.get('codeinput')
    with open('../compiler/testfile.txt','w') as f:
        f.write(content)
        f.close()
    os.chdir('../compiler')
    cmd = r'compiler.exe testfile.txt -o out.txt'
    rs = os.popen(cmd)
    res = rs.read()
    pcode = ''
    with open('../compiler/out.txt') as f:
        for line in f.readlines():
            pcode += str(line).replace('\t','&nbsp;&nbsp;&nbsp;&nbsp;').replace(' ','&nbsp;')+'</br>'
    os.chdir('../compiler')
    cmd2 = r'pcode_interpreter.exe'
    rs = os.popen(cmd2)
    pcodeout = ''
    for line in rs.readlines():
        pcodeout += str(line).replace('\t','&nbsp;&nbsp;&nbsp;&nbsp;')+'</br>'
    print(pcodeout)
    # using placeholding image here for now
    #img = Image.open('../webservice/placeholder.png')
    #img = img.convert('RGB')
    #img_b64 = b"data:image/jpeg;base64,"+pil_base64(img)
    return jsonify({'pcode': pcode, 'output':pcodeout})


if __name__ == "__main__":
    #build_compiler()
    # webbrowser.open_new_tab('http://127.0.0.1:5000')
    app.run(debug=True)