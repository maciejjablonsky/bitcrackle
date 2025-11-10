

export module utilities;

export struct non_copyable
{
    non_copyable(const non_copyable&)            = delete;
    non_copyable& operator=(const non_copyable&) = delete;
};
