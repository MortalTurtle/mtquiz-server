
username = "test"
password = "qwerty"
auth_header_name = "X-User-Auth-Token"
group_name = "testgroup"
group_description = "testdescription"
test_name = "testtestname"
test_description = "testtestdescription"


async def setup_user(service_client):
    data = {"username": username, "password": password}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    return response.text


async def setup_user_login(service_client):
    data = {"username": username, "password": password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    return response.text


async def setup_group(service_client):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    data = {"name": group_name, "description": group_description}
    response = await service_client.post(
        '/v1/groups',
        headers=header,
        json=data
    )
    return response.text


async def setup_test(service_client, group_id):
    auth_token = await setup_user_login(service_client)
    header = {auth_header_name: auth_token}
    data = {"name": test_name, "description": test_description}
    response = await service_client.post(
        '/v1/groups/' + group_id + '/tests',
        headers=header,
        json=data
    )
    return response.text
