create table if not exists users (
    user_id bigint primary key not null,
    user_name text,
    last_grow bigint,
    rank bigint,
    cash bigint,
    buff bigint
);

create table if not exists watermelons (
    id bigserial primary key not null,
    name text,
    description text,
    cost bigint
);

create table if not exists manure (     --- навоз
    id bigserial primary key not null,
    name text,
    description text,
    cost  bigint
);

create table if not exists watermelons_count (
    user_id bigint not null,
    watermelon_id bigint not null,
    watermelon_count bigint not null,
    foreign key (user_id) references users(user_id),
    foreign key (watermelon_id) references watermelons(id)
);

create table if not exists manures_count (
    user_id bigint not null,
    manure_id bigint not null,
    manure_count bigint not null,
    foreign key (user_id) references users(user_id),
    foreign key (manure_id) references manure(id)
);

create table if not exists post (
    user_id bigint primary key not null, 
    is_active boolean,
    watermelon_type text
);