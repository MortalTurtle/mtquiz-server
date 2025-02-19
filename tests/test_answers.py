import tests.setup_for_tests as setup_for_tests


async def test_answers_add_get_choose_single(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=[{"questionId": question_id, "answer": "answer1"},
              {"questionId": question_id, "answer": "answer2"}]
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 2


async def test_ans_add_choose_single_multiple_true(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}, {
        "questionId": question_id, "answer": "answer2"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 400
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) == 0


async def test_ans_add_choose_single_multiple_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}, {
        "questionId": question_id, "answer": "answer2"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        json=true_answers
    )
    assert response.status == 401
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) == 0


async def test_answers_add_get_choose_multiple(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseMultiple")
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}, {
        "questionId": question_id, "answer": "answer2"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 2
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=[{"questionId": question_id, "answer": "answer1"},
              {"questionId": question_id, "answer": "answer2"}]
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 2


async def test_answers_add_get_write(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="Write")
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=[{"questionId": question_id, "answer": "answer1"},
              {"questionId": question_id, "answer": "answer2"}]
    )
    assert response.status == 400
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 0


async def test_answers_add_get_write_wrong_n_false_ans(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="Write")
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer1"}, {
        "questionId": question_id, "answer": "answer2"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=true_answers
    )
    assert response.status == 400
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 0


async def test_answers_get_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseMultiple")
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id
    )
    assert response.status == 401


async def test_answers_add_no_role(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auto_user2_id = await setup_for_tests.setup_user(service_client,
                                                     "name2",
                                                     "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client,
                                                        "name2",
                                                        "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 403


async def test_answers_add_wrong_role(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auto_user2_id = await setup_for_tests.setup_user(service_client,
                                                     "name2",
                                                     "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client,
                                                        "name2",
                                                        "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    true_answers = [{"questionId": question_id, "answer": "answer"}]
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=true_answers
    )
    assert response.status == 403


async def test_answers_replace(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseMultiple")
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    data = []
    response = await service_client.put(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.put(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) == 0
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id + "&falseAnswers=",
        headers=header
    )
    response_json = response.json()
    assert len(response_json) == 0


async def test_answers_replace_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseMultiple")
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    data = []
    response = await service_client.put(
        '/v1/answers?questionId=' + question_id,
        json=data
    )
    assert response.status == 401
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) != 0


async def test_answers_replace_user_not_joined(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auto_user2_id = await setup_for_tests.setup_user(service_client,
                                                     "name2",
                                                     "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client,
                                                        "name2",
                                                        "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    data = []
    response = await service_client.put(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=data
    )
    assert response.status == 403
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) != 0


async def test_answers_replace_user_wrong_role(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auto_user2_id = await setup_for_tests.setup_user(service_client,
                                                     "name2",
                                                     "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client,
                                                        "name2",
                                                        "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    question_id = await setup_for_tests.setup_question(service_client, test_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    data = []
    response = await service_client.put(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=data
    )
    assert response.status == 403
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) != 0


async def test_answers_replace_no_question(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseMultiple")
    header = {setup_for_tests.auth_header_name: auth_token}
    await setup_for_tests.setup_answers(service_client, question_id)
    data = []
    response = await service_client.put(
        '/v1/answers?questionId=' + 'questionId',
        headers=header,
        json=data
    )
    assert response.status == 400
    response = await service_client.get(
        '/v1/answers?questionId=' + question_id,
        headers=header
    )
    response_json = response.json()
    assert len(response_json) != 0
