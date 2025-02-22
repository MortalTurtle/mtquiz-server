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


async def test_answers_submit(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseSingle",
                                                       weight=1)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id,
        headers=header,
        json=[{"questionId": question_id, "answer": "answer"}]
    )
    question_id2 = await setup_for_tests.setup_question(service_client,
                                                        test_id,
                                                        type="ChooseMultiple",
                                                        weight=1)
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id2,
        headers=header,
        json=[{"questionId": question_id2, "answer": "answer"}]
    )
    question_id3 = await setup_for_tests.setup_question(service_client,
                                                        test_id,
                                                        type="Write",
                                                        weight=1)
    response = await service_client.post(
        '/v1/answers?questionId=' + question_id3,
        headers=header,
        json=[{"questionId": question_id3, "answer": "answer"}]
    )
    answers_to_submit = [
        {"questionId": question_id, "answer": "ANSWer"},
        {"questionId": question_id2, "answer": "answER"},
        {"questionId": question_id3, "answer": "ANSWER"},
    ]
    response = await service_client.post(
        '/v1/answers/submit?testId=' + test_id,
        headers=header,
        json=answers_to_submit
    )
    assert response.status == 200
    response_json = response.json()
    assert "userScore" in response_json
    assert response_json["userScore"] == 3
    assert "minScoreToPass" in response_json
    assert response_json["minScoreToPass"] == 3


async def test_answers_submit_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseSingle",
                                                       weight=1)
    header = {setup_for_tests.auth_header_name: auth_token}
    answers_to_submit = []
    response = await service_client.post(
        '/v1/answers/submit?testId=' + test_id,
        json=answers_to_submit
    )
    assert response.status == 401


async def test_answers_submit_no_test(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseSingle",
                                                       weight=1)
    header = {setup_for_tests.auth_header_name: auth_token}
    answers_to_submit = []
    response = await service_client.post(
        '/v1/answers/submit?testId=' + "WRONGTESTID",
        headers=header,
        json=answers_to_submit
    )
    assert response.status == 404


async def test_answers_submit_user_has_no_role_in_group(service_client):
    id = await setup_for_tests.setup_user(service_client)
    id2 = await setup_for_tests.setup_user(service_client, "name2", "pswd2")
    auth_token = await setup_for_tests.setup_user_login(service_client,
                                                        "name2", "pswd2")
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id,
                                               min_score=3)
    question_id = await setup_for_tests.setup_question(service_client,
                                                       test_id,
                                                       type="ChooseSingle",
                                                       weight=1)
    header = {setup_for_tests.auth_header_name: auth_token}
    answers_to_submit = []
    response = await service_client.post(
        '/v1/answers/submit?testId=' + test_id,
        headers=header,
        json=answers_to_submit
    )
    assert response.status == 403


async def test_answers_get_results(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id2 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id3 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    for i in range(10):
        await setup_for_tests.setup_results(service_client, test_id,
                                            question_id,
                                            question_id2,
                                            question_id3)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/results?limit=5&offset=5',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 5
    for i in range(5):
        assert response_json[i]["userId"] == id
        assert response_json[i]["testId"] == test_id
        assert response_json[i]["score"] == 3


async def test_answers_get_results_different_tests(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    test_id2 = await setup_for_tests.setup_test(service_client,
                                                group_id,
                                                min_score=1)
    question_id_from_test_2 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id2,
        weight=1)
    await service_client.post(
        '/v1/answers/submit?testId=' + test_id2,
        headers=header,
        json=[{"questionId": question_id_from_test_2, "answer": "answer"}]
    )
    question_id = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id2 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id3 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    await setup_for_tests.setup_results(service_client, test_id,
                                        question_id,
                                        question_id2, question_id3)
    response = await service_client.get(
        '/v1/results',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 2
    assert response_json[0]["score"] == 3
    assert response_json[0]["testId"] == test_id
    assert response_json[1]["score"] == 1
    assert response_json[1]["testId"] == test_id2
    response = await service_client.get(
        '/v1/results?testId=' + test_id2,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    assert response_json[0]["score"] == 1


async def test_answers_get_results_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    await setup_for_tests.join_group(service_client, group_id, auth_token)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id2 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id3 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    await setup_for_tests.setup_results(service_client, test_id,
                                        question_id,
                                        question_id2, question_id3)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/results',
    )
    assert response.status == 401


async def test_answers_get_results_user_not_joined(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client,
                                               group_id, min_score=3)
    question_id = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id2 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    question_id3 = await setup_for_tests.setup_question_with_answer(
        service_client,
        test_id,
        type="ChooseSingle",
        weight=1)
    await setup_for_tests.setup_results(service_client, test_id,
                                        question_id,
                                        question_id2, question_id3)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/results',
        headers=header
    )
    assert response.status == 400
