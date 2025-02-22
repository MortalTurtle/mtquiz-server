import tests.setup_for_tests as setup_for_tests


async def test_test_create(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": setup_for_tests.test_name,
            "description": setup_for_tests.test_description}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    assert response.status == 200
    assert response.text != ""


async def test_test_create_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    data = {"name": setup_for_tests.test_name,
            "description": setup_for_tests.test_description}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        json=data
    )
    assert response.status == 401


async def test_test_create_missing_parameters_description(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": setup_for_tests.test_name}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_test_create_missing_name_parameter(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"description": setup_for_tests.test_description}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_test_create_wrong_group_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": setup_for_tests.test_name,
            "description": setup_for_tests.test_description}
    response = await service_client.post(
        '/v1/groups/' + "wronggroupid" + '/tests',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_test_get_test(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert 'id' in response_json
    assert 'groupId' in response_json
    assert response_json["groupId"] == group_id
    assert 'ownerId' in response_json
    assert response_json["ownerId"] == id
    assert response_json["name"] == setup_for_tests.test_name
    assert response_json["description"] == setup_for_tests.test_description


async def test_test_get_test_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id
    )
    assert response.status == 401


async def test_test_get_test_wrongId(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + "wrongId",
        headers=header
    )
    assert response.status == 404


async def test_test_get_tests(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test1_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": "newtestname",
            "description": "newtestdescription"}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    test2_id = response.text
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 2
    assert "id" in response_json[0]
    assert "groupId" in response_json[0]
    assert response_json[0]["groupId"] == group_id
    assert "ownerId" in response_json[0]
    assert response_json[0]["ownerId"] == id
    assert "name" in response_json[0]
    assert "description" in response_json[0]


async def test_test_get_tests_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests'
    )
    assert response.status == 401


async def test_test_get_tests_wrong_id(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.get(
        '/v1/groups/' + 'wrongId' + '/tests',
        headers=header
    )
    assert response.status == 400


async def test_test_get_tests_paging(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    for i in range(10):
        data = {"name": setup_for_tests.test_name + str(i),
                "description": setup_for_tests.test_description}
        response = await service_client.post(
            '/v1/groups/' + group_id + '/tests',
            headers=header,
            json=data
        )
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests?limit=5',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    j = 9
    for i in range(5):
        assert response_json[i]["name"] == setup_for_tests.test_name + str(j)
        j -= 1

    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests?limit=5&offset=5',
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    for i in range(5):
        assert response_json[i]["name"] == setup_for_tests.test_name + str(j)
        j -= 1


async def test_test_edit(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": "newtestname",
            "description": "newtestdescription", "minScoreToPass": 2}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] == "newtestname"
    assert response_json["description"] == "newtestdescription"
    assert response_json["minScore"] == 2


async def test_test_edit_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    data = {"name": "newtestname",
            "description": "newtestdescription", "minScoreToPass": 2}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        json=data
    )
    assert response.status == 401


async def test_test_wrong_role(service_client):
    id = await setup_for_tests.setup_user(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    data = {"username": 'user2', "password": "password2"}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 200
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200
    auth_token = response.text
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.post(
        '/v1/groups/'+group_id+'/join',
        headers=header
    )
    assert response.status == 200
    data = {"name": "newtestname",
            "description": "newtestdescription", "minScoreToPass": 2}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 403


async def test_test_edit_edit_name(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": "newtestname"}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] == "newtestname"


async def test_test_edit_description(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"description": "newtestdescription"}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["description"] == "newtestdescription"


async def test_test_edit_min_score(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"minScoreToPass": 2}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["minScore"] == 2


async def test_test_edit_negative_min_score(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    group_id = await setup_for_tests.setup_group(service_client)
    test_id = await setup_for_tests.setup_test(service_client, group_id)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": "newtestname",
            "description": "newtestdescription", "minScoreToPass": -2}
    response = await service_client.patch(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header,
        json=data
    )
    assert response.status == 400
    response = await service_client.get(
        '/v1/groups/' + group_id + '/tests/' + test_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] != "newtestname"
    assert response_json["description"] != "newtestdescription"
