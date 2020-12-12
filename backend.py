from flask import Flask, request, render_template, jsonify

app = Flask(__name__, template_folder="./")


class status():
    angle = 0
    no_of_times = 0
    torque = 0
    realtime_angle = 0


command = status()


@app.route('/', methods=['GET', 'POST'])
def my_form():
    if(request.method == 'GET'):
        return render_template('main1.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        no = request.form["no_of_times"]
        torq = request.form["torque"]
        command.angle = ang
        command.no_of_times = no
        command.torque = torq
        return render_template('main1.html')


@app.route("/angle", methods=['GET'])
def show_res1():
    if(request.method == 'GET'):
        return str(command.angle)


@app.route("/no", methods=['GET'])
def show_res2():
    if(request.method == 'GET'):
        return str(command.no_of_times)


@app.route("/torque", methods=['GET'])
def show_res3():
    if(request.method == 'GET'):
        return str(command.torque)


@app.route("/angle_realtime", methods=['GET', 'POST'])
def post_angle():
    if(request.method == 'POST'):
        content = request.get_json()
        command.realtime_angle = content['angle']
        return str(command.realtime_angle)
    if(request.method == 'GET'):
        return render_template("result.html", result=command.realtime_angle)


@app.route("/data", methods=['GET'])
def show_data():
    if(request.method == 'GET'):
        data = []
        data.append(
            {
                "angle": command.angle,
                "no": command.no_of_times,
                "torque": command.torque,

            }
        )
        return jsonify(
            category="success",
            data=data,
            status=200
        )


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
