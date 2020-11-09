let keywords = [
    {
        "+":{
            "name":"add",
            "type":"operator",
        }
    },
    {
        "=":{
            "name":"equals",
            "type":"operator",
        }
    },
    {
        "-":{
            "name":"minus",
            "type":"operator",
        }
    },
    {
        "*":{
            "name":"multiply",
            "type":"operator",
        }
    },
    {
        "/":{
            "name":"divide",
            "type":"operator",
        }
    },

]
let operators = {
    "color":"purple",
    "elements":["+","-","*","/","="]
}
let s = "int _ab0_23 = 333"
let handleChange = (e)=>{
    let codeinput = $("#codeinput")
    let t = codeinput.text()
    console.log(t)
    codeinput.html('<span>t</span>')
}
let regex = "^[_a-zA-Z][_a-zA-Z0-9]*$"
handlekeydown = (e) =>{
    console.log(e)
}