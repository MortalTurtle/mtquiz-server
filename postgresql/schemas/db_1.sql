CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

DROP SCHEMA IF EXISTS quizdb CASCADE;

CREATE SCHEMA IF NOT EXISTS quizdb;

CREATE TYPE quizdb.group_role AS ENUM('Owner', 'Contributor', 'Participant');

CREATE TYPE quizdb.question_type AS ENUM('ChooseSingle', 'ChooseMultiple', 'Write');

CREATE TABLE IF NOT EXISTS quizdb.groups(
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    name TEXT NOT NULL,
    description TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS quizdb.users(
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    username VARCHAR(100) NOT NULL,
    password_hash TEXT NOT NULL,
    group_id TEXT,
    UNIQUE(username)
);

CREATE TABLE IF NOT EXISTS quizdb.session_tokens(
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id TEXT REFERENCES quizdb.users(id)
);

CREATE TABLE IF NOT EXISTS quizdb.group_users(
    group_id TEXT REFERENCES quizdb.groups(id),
    user_id TEXT REFERENCES quizdb.users(id),
    role quizdb.group_role,
    UNIQUE(group_id, user_id)
);

CREATE TABLE IF NOT EXISTS quizdb.tests(
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    group_id TEXT REFERENCES quizdb.groups(id),
    owner_id TEXT REFERENCES quizdb.users(id),
    name TEXT NOT NULL,
    min_score INTEGER,
    description TEXT NOT NULL,
    created_ts TIMESTAMP DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_by_created_ts_tests ON quizdb.tests(created_ts);

CREATE TABLE IF NOT EXISTS quizdb.question_types_description(
    type quizdb.question_type,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    UNIQUE(type),
    UNIQUE(name)
);

INSERT INTO quizdb.question_types_description(type, name, description) VALUES
('ChooseSingle', 'Choose Single', 'You have to choose the right answer'),
('ChooseMultiple', 'Choose Multiple', 'You have to choose the right answers'),
('Write', 'Write', 'You have to write the right answer');

CREATE TABLE IF NOT EXISTS quizdb.test_questions(
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    test_id TEXT REFERENCES quizdb.tests(id),
    type quizdb.question_type,
    weight INTEGER,
    text TEXT NOT NULL,
    created_ts TIMESTAMP DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_by_created_ts_questions ON quizdb.test_questions(created_ts);

CREATE TABLE IF NOT EXISTS quizdb.question_answers(
    question_id TEXT REFERENCES quizdb.test_questions(id),
    answer TEXT NOT NULL,
    UNIQUE(question_id, answer)
);

CREATE TABLE IF NOT EXISTS quizdb.question_false_answers(
    question_id TEXT REFERENCES quizdb.test_questions(id),
    answer TEXT NOT NULL,
    UNIQUE(question_id, answer)
);
CREATE INDEX IF NOT EXISTS idx_by_question_id_answers ON quizdb.question_answers(question_id);
CREATE INDEX IF NOT EXISTS idx_by_question_id_false_answers ON quizdb.question_false_answers(question_id);

CREATE TABLE IF NOT EXISTS quizdb.test_results(
    user_id TEXT REFERENCES quizdb.users(id),
    test_id TEXT REFERENCES quizdb.tests(id),
    score INTEGER NOT NULL,
    finished_ts TIMESTAMP DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_by_finished_ts_results ON quizdb.test_results(finished_ts);