
username = "test"
password = "qwerty"
auth_header_name = "X-User-Auth-Token"
group_name = "testgroup"
group_description = "testdescription"
test_name = "testtestname"
test_description = "testtestdescription"
question_text = "questiontext"
question_type = "ChooseSingle"


async def setup_user(service_client, name=username, pswd=password):
    data = {"username": name, "password": pswd}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    return response.text


async def setup_user_login(service_client, name=username, pswd=password):
    data = {"username": name, "password": pswd}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    return response.text


async def setup_group(service_client,
                      name=group_name, description=group_description):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    data = {"name": name, "description": description}
    response = await service_client.post(
        '/v1/groups',
        headers=header,
        json=data
    )
    return response.text


async def setup_test(service_client, group_id,
                     name=test_name, description=test_description):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    data = {"name": name, "description": description}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    return response.text


async def setup_question(service_client, test_id,
                         text=question_text, type=question_type):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    data = {"text": text, "type": type}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header,
        json=data
    )
    return response.text


async def join_group(service_client, group_id, auth_token):
    header = {auth_header_name: auth_token}
    response = await service_client.post(
        '/v1/groups/'+group_id+'/join',
        headers=header
    )


async def setup_answers(service_client, question_id):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    false_answers = [{"questionId": question_id, "answer": "answer1"}, {
        "questionId": question_id, "answer": "answer2"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=false_answers
    )
