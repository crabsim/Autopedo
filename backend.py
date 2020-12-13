from flask import Flask, redirect, url_for, request, render_template, jsonify

app = Flask(__name__, template_folder="./")


class status():
    angle = 0
    no_of_times = 0
    torque = 0
    realtime_angle = 0
    page = ""
    new_data = "No"
    part = 0
    instruction = 0


command = status()


@app.route("/", methods=['GET', 'POST'])
def show_demo():
    if(request.method == 'GET'):
        return render_template('demo.html')
    if(request.method == 'POST'):
        user_answer = request.form['activity']
        command.page = user_answer
        command.new_data = "Yes"
        if(user_answer == "arm"):
            return redirect(url_for('my_formarm'))
        elif(user_answer == "leg"):
            return redirect(url_for('my_formleg'))
        elif(user_answer == "hand"):
            return redirect(url_for('my_formhand'))
        elif(user_answer == "back"):
            return redirect(url_for('my_formback'))
        else:
            return render_template('demo.html')


@app.route('/arm', methods=['GET', 'POST'])
def my_formarm():
    if(request.method == 'GET'):
        return render_template('main.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        no = request.form["no_of_times"]
        torq = request.form["torque"]
        command.angle = ang
        command.no_of_times = no
        command.torque = torq
        return render_template('result.html')


@app.route('/hand', methods=['GET', 'POST'])
def my_formhand():
    if(request.method == 'GET'):
        return render_template('hand.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        no = request.form["no_of_times"]
        torq = request.form["torque"]
        command.angle = ang
        command.no_of_times = no
        command.torque = torq
        return render_template('result.html')


@app.route('/back', methods=['GET', 'POST'])
def my_formback():
    if(request.method == 'GET'):
        return render_template('main.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        torq = request.form["torque"]
        command.angle = ang
        command.torque = torq
        return render_template('result.html')


@app.route('/leg', methods=['GET', 'POST'])
def my_formleg():
    if(request.method == 'GET'):
        return render_template('main.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        no = request.form["no_of_times"]
        torq = request.form["torque"]
        command.angle = ang
        command.no_of_times = no
        command.torque = torq
        return render_template('result.html')


@app.route('/hand', methods=['GET', 'POST'])
def my_form():
    if(request.method == 'GET'):
        return render_template('main.html')
    if(request.method == 'POST'):
        ang = request.form["angle"]
        no = request.form["no_of_times"]
        torq = request.form["torque"]
        command.angle = ang
        command.no_of_times = no
        command.torque = torq
        return render_template('result.html')


@app.route("/angle", methods=['GET', 'POST'])
def show_res1():
    if(request.method == 'GET'):
        return str(command.angle)
    if(request.method == 'POST'):
        content = request.get_json()
        command.angle = content['angle']


@app.route("/curve", methods=['GET', 'POST'])
def show_curve():
    if(request.method == 'GET'):
        return str(command.angle)
    if(request.method == 'POST'):
        content = request.get_json()
        command.angle = content['angle']


@app.route("/angle", methods=['GET', 'POST'])
def show_leg():
    if(request.method == 'GET'):
        return str(command.angle)
    if(request.method == 'POST'):
        content = request.get_json()
        command.angle = content['angle']


@app.route("/hand", methods=['GET', 'POST'])
def show_hand():
    if(request.method == 'GET'):
        return str(command.angle)
    if(request.method == 'POST'):
        content = request.get_json()
        command.angle = content['angle']


# @app.route("/arm", methods=['GET'])
# def show_arm():
#     if(request.method == 'GET'):
#         return render_template('main1.html')


@app.route("/no", methods=['GET'])
def show_res2():
    if(request.method == 'GET'):
        return str(command.no_of_times)


@app.route("/status", methods=['GET'])
def show_status():
    if(request.method == 'GET'):
        data = []
        data.append(
            {
                "NewData": command.new_data,
                "Part": command.part,
                "Inst": command.instruction,

            }
        )
        return jsonify(
            category="success",
            data=data,
            status=200
        )


@app.route("/torque", methods=['GET'])
def show_res3():
    if(request.method == 'GET'):
        return str(command.torque)


@app.route("/realtime_angle", methods=['GET', 'POST'])
def post_angle():
    if(request.method == 'POST'):
        content = request.get_json()
        command.realtime_angle = content['angle']
        command.instruction = 1
        return str(command.realtime_angle)
    if(request.method == 'GET'):
        return render_template("result.html", result=command.realtime_angle)


@app.route("/realtime_curve", methods=['GET', 'POST'])
def post_curve():
    if(request.method == 'POST'):
        content = request.get_json()
        command.realtime_angle = content['curve']
        command.instruction = 1
        return str(command.realtime_angle)
    if(request.method == 'GET'):
        return render_template("result.html", result=command.realtime_angle)


@app.route("/realtime_leg", methods=['GET', 'POST'])
def post_leg():
    if(request.method == 'POST'):
        content = request.get_json()
        command.realtime_angle = content['angle']
        command.instruction = 1
        return str(command.realtime_angle)
    if(request.method == 'GET'):
        return render_template("result.html", result=command.realtime_angle)


@app.route("/realtime_hand", methods=['GET', 'POST'])
def post_hand():
    if(request.method == 'POST'):
        content = request.get_json()
        command.realtime_angle = content['angle']
        command.instruction = 1
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
