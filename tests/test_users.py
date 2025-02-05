# Start via `make test-debug` or `make test-release`
import tests.setup_for_tests as setup_for_tests


async def test_user_get(service_client):
    id = await setup_for_tests.setup_user(service_client)
    response = await service_client.get(
        'v1/users?id=' + id
    )
    response_json = response.json()
    assert response.status == 200
    assert response_json["username"] == setup_for_tests.username
    assert response_json["id"] == id
    response = await service_client.get(
        'v1/users?username=' + setup_for_tests.username
    )
    response_json = response.json()
    assert response.status == 200
    assert response_json["username"] == setup_for_tests.username
    assert response_json["id"] == id


async def test_user_get_something_wrong_with_parameters(service_client):
    id = await setup_for_tests.setup_user(service_client)
    response = await service_client.get(
        'v1/users'
    )
    assert response.status == 400
    response = await service_client.get(
        'v1/users?id=wrongId'
    )
    assert response.status == 404
    response = await service_client.get(
        'v1/users?username=wrongUsername'
    )
    assert response.status == 404

# TODO do :)


async def test_user_get_joined_group(service_client):
    pass


async def test_user_add(service_client):
    data = {"username": setup_for_tests.username,
            "password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 200


async def test_user_add_no_pswd(service_client):
    data = {"username": setup_for_tests.username}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 400


async def test_user_add_no_username(service_client):
    data = {"password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 400


async def test_user_login_no_pswd(service_client):
    id = await setup_for_tests.setup_user(service_client)
    data = {"username": setup_for_tests.username}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 400


async def test_user_login(service_client):
    id = await setup_for_tests.setup_user(service_client)
    data = {"username": setup_for_tests.username,
            "password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200
    assert response.text != ""


async def test_user_login_no_username(service_client):
    id = await setup_for_tests.setup_user(service_client)
    data = {"password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 400


async def test_user_login_user_not_found(service_client):
    id = await setup_for_tests.setup_user(service_client)
    data = {"username": "wrongUsername)", "password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 404


async def test_user_patch_username(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"username": "newusername"}
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.patch(
        '/v1/users',
        json=data,
        headers=header
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/users',
        params={"username": "newusername"}
    )
    assert response.status == 200
    response_json = response.json()
    assert response_json["username"] == "newusername"
    assert response_json["id"] == id
    data = {"username": "newusername", "password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200


async def test_user_patch_password(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"password": "newpassword"}
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.patch(
        '/v1/users',
        json=data,
        headers=header
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/users',
        params={"username": setup_for_tests.username}
    )
    assert response.status == 200
    response_json = response.json()
    assert response_json["id"] == id
    data = {"username": setup_for_tests.username, "password": "newpassword"}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200


async def test_user_patch_both(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"username": "newusername", "password": "newpassword"}
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.patch(
        '/v1/users',
        json=data,
        headers=header
    )
    assert response.status == 200
    response = await service_client.get(
        '/v1/users',
        params={"username": "newusername"}
    )
    assert response.status == 200
    response_json = response.json()
    assert response_json["username"] == "newusername"
    assert response_json["id"] == id
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200


async def test_user_patch_no_args(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {}
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.patch(
        '/v1/users',
        json=data,
        headers=header
    )
    assert response.status == 400


async def test_user_patch_no_header(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"username": "newusername", "password": "newpassword"}
    response = await service_client.patch(
        '/v1/users',
        json=data
    )
    assert response.status == 401


async def test_user_patch_no_username_taken_password_has_not_changed(service_client):
    id = await setup_for_tests.setup_user(service_client)
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"username": "newusername", "password": "qwerty"}
    header = {setup_for_tests.auth_header_name: auth_token}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    auth_token = await setup_for_tests.setup_user_login(service_client)
    data = {"username": "newusername", "password": "newpassword"}
    response = await service_client.patch(
        '/v1/users',
        json=data,
        headers=header
    )
    assert response.status == 400
    data = {"username": setup_for_tests.username,
            "password": setup_for_tests.password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    assert response.status == 200
