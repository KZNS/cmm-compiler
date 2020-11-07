from flask import Flask,render_template,jsonify,request
import os
import subprocess
from PIL import Image
import base64
from io import BytesIO
app = Flask(__name__)
logging_levels = []

def pil_base64(image):
    img_buffer = BytesIO()
    image.save(img_buffer,format="jpeg")
    byte_data = img_buffer.getvalue()
    base64_str = base64.b64encode(byte_data)
    return base64_str


@app.route('/',methods=["GET"])
def main():
    return render_template("index.html")


@app.route('/',methods=["POST"])
def comp():
    r = request.get_json()
    logging = r.get('logging')=='on'
    if logging:
        logging_level = r.get('loggingLevel')
        if logging_level not in logging_levels:
            logging_level = 'info'
    content = r.get('codeinput')
    cmd = r'1.exe'
    rs = os.popen(cmd)
    res = rs.read()
    ## using placeholding image here for now
    img = Image.open('./placeholder.png')
    img = img.convert('RGB')
    img_b64 = b"data:image/jpeg;base64,"+pil_base64(img)
    return jsonify({'data':res,'img':img_b64})

if __name__ == "__main__":
    app.run(debug=True)