from flask import Flask,render_template
import os
import subprocess

app = Flask(__name__)

@app.route('/',methods=["GET"])
def main():
    return render_template("index.html",info="NO INPUT")


@app.route('/',methods=["POST"])
def comp():
    cmd = r'1.exe'
    rs = os.popen(cmd)
    res = rs.read()
    #print(res)
    return render_template("index.html",info=res)

if __name__ == "__main__":
    app.run(debug=True)