import tests.setup_for_tests as setup_for_tests


async def test_question_create(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"text": setup_for_tests.question_text,
            "type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header,
        json=data
    )
    assert response.status == 200
    assert response.text != ""


async def test_question_create_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    data = {"text": setup_for_tests.question_text,
            "type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        json=data
    )
    assert response.status == 401
    assert response.text == ""


async def test_question_create_wrong_test_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"text": setup_for_tests.question_text,
            "type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/tests/' + 'wrongtestid' + '/questions',
        headers=header,
        json=data
    )
    assert response.status == 400
    assert response.text != ""


async def test_question_create_user_in_wrong_group(service_client):
    id = await setup_for_tests.setup_user(service_client)
    user2_id = await setup_for_tests.setup_user(service_client,
                                                "user2",
                                                "pswd2")
    auth_token2 = await setup_for_tests.setup_user_login(service_client,
                                                         "user2",
                                                         "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    header2 = {setup_for_tests.auth_header_name: auth_token2}
    data = {"text": setup_for_tests.question_text,
            "type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header2,
        json=data
    )
    assert response.status == 403
    assert response.text == ""


async def test_question_create_user_wrong_role(service_client):
    id = await setup_for_tests.setup_user(service_client)
    user2_id = await setup_for_tests.setup_user(service_client,
                                                "user2",
                                                "pswd2")
    auth_token2 = await setup_for_tests.setup_user_login(service_client,
                                                         "user2",
                                                         "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header2 = {setup_for_tests.auth_header_name: auth_token2}
    data = {"text": setup_for_tests.question_text,
            "type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/join',
        headers=header2
    )
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header2,
        json=data
    )
    assert response.status == 403
    assert response.text == ""


async def test_question_create_no_text(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"type": setup_for_tests.question_type}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_question_create_no_type(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"text": setup_for_tests.question_text}
    response = await service_client.post(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_question_get(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions/' + question_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert "text" in response_json
    assert response_json["text"] == setup_for_tests.question_text
    assert "type" in response_json
    assert response_json["type"] == setup_for_tests.question_type
    assert "testId" in response_json
    assert response_json["testId"] == test_id
    assert "id" in response_json
    assert response_json["id"] == question_id


async def test_question_get_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions/' + question_id
    )
    assert response.status == 401


async def test_question_get_wrong_test_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + 'wrongtestid' + '/questions/' + question_id,
        headers=header
    )
    assert response.status == 400


async def test_question_get_wrong_question_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions/' + 'wrongquestionid',
        headers=header
    )
    assert response.status == 404


async def test_question_get_not_joined_group(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions/' + question_id,
        headers=header
    )
    assert response.status == 403


async def test_questions_get(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"text": "questiontext", "type": "ChooseSingle"}
    for i in range(5):
        response = await service_client.post(
            '/v1/groups/tests/' + test_id + '/questions',
            headers=header,
            json=data
        )
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 5


async def test_questions_get_test_paging(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    for i in range(10):
        data = {"text": "questiontext" + str(i + 1), "type": "ChooseSingle"}
        response = await service_client.post(
            '/v1/groups/tests/' + test_id + '/questions',
            headers=header,
            json=data
        )
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions?limit=5',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 5
    j = 10
    for i in range(5):
        assert response_json[i]["text"] == "questiontext" + str(j)
        j -= 1
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions?limit=5&offset=5',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 5
    for i in range(5):
        assert response_json[i]["text"] == "questiontext" + str(j)
        j -= 1


async def test_questions_get_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions'
    )
    assert response.status == 401


async def test_questions_get_wrong_test_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + 'wrongtestid' + '/questions',
        headers=header
    )
    assert response.status == 400


async def test_questions_get_negative_limit(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions?limit=-4',
        headers=header
    )
    assert response.status == 400


async def test_questions_get_negative_offset(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions?offset=-3',
        headers=header
    )
    assert response.status == 400


async def test_questions_get_user_not_in_group(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/tests/' + test_id + '/questions',
        headers=header
    )
    assert response.status == 403
