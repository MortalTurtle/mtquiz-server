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
