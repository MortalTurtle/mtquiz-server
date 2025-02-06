import tests.setup_for_tests as setup_for_tests
import pytest

from testsuite.databases import pgsql


async def test_group_create(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": setup_for_tests.group_name,
            "description": setup_for_tests.group_description}
    response = await service_client.post(
        '/v1/groups',
        headers=header,
        json=data
    )
    assert response.status == 200
    assert response.text != ""


async def test_group_create_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    data = {"name": setup_for_tests.group_name,
            "description": setup_for_tests.group_description}
    response = await service_client.post(
        '/v1/groups',
        json=data
    )
    assert response.status == 401


async def test_group_create_name_missing(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"description": setup_for_tests.group_description}
    response = await service_client.post(
        '/v1/groups',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_group_create_description_missing(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"description": setup_for_tests.group_description}
    response = await service_client.post(
        '/v1/groups',
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_group_get(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    response = await service_client.get(
        '/v1/groups/' + group_id,
        headers=header
    )
    assert response.status == 200
    response_json = response.json()
    assert 'id' in response_json
    assert 'name' in response_json
    assert 'description' in response_json


async def test_group_get_wrong_id(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    response = await service_client.get(
        '/v1/groups/wrongId',
        headers=header
    )
    assert response.status == 404


async def test_group_join(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    response = await service_client.post(
        '/v1/groups/'+group_id+'/join',
        headers=header
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/users?id=' + user_id
    )
    response_json = response.json()
    assert "groupId" in response_json
    assert response_json["groupId"] == group_id


async def test_group_join_wrong_group(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    response = await service_client.post(
        '/v1/groups/wrongId/join',
        headers=header
    )
    assert response.status == 404
    response = await service_client.get(
        '/v1/users?id=' + user_id
    )
    response_json = response.json()
    assert "groupId" not in response_json


async def test_group_join_no_header(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    response = await service_client.post(
        '/v1/groups/'+group_id+'/join'
    )
    assert response.status == 401


async def test_group_edit(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    data = {"name": "newgroupname", "description": 'newgroupdescription'}
    response = await service_client.patch(
        '/v1/groups/' + group_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] == "newgroupname"
    assert response_json["description"] == "newgroupdescription"


async def test_group_edit_only_name(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    data = {"name": "newgroupname"}
    response = await service_client.patch(
        '/v1/groups/' + group_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] == "newgroupname"
    assert response_json["description"] == setup_for_tests.group_description


async def test_group_edit_only_description(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    data = {"description": 'newgroupdescription'}
    response = await service_client.patch(
        '/v1/groups/' + group_id,
        headers=header,
        json=data
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/groups/' + group_id,
        headers=header
    )
    response_json = response.json()
    assert response_json["name"] == setup_for_tests.group_name
    assert response_json["description"] == "newgroupdescription"


async def test_group_edit_no_header(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    data = {"name": "newgroupname", "description": 'newgroupdescription'}
    response = await service_client.patch(
        '/v1/groups/' + group_id,
        json=data
    )
    assert response.status == 401


async def test_group_edit_no_parameters(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    group_id = await setup_for_tests.setup_group(service_client)
    data = {}
    response = await service_client.patch(
        '/v1/groups/' + group_id,
        headers=header,
        json=data
    )
    assert response.status == 400


async def test_group_edit_wrong_id(service_client):
    user_id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    header = {setup_for_tests.auth_header_name: auth_token}
    data = {"name": "newgroupname", "description": 'newgroupdescription'}
    response = await service_client.patch(
        '/v1/groups/wrongid',
        headers=header,
        json=data
    )
    assert response.status == 404
