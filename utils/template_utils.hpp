#ifndef TEMPLATEUTILS_HPP_
#define TEMPLATEUTILS_HPP_

template <template <typename...> class T, template <typename...> class U> struct is_same_template : std::false_type
{};

template <template <typename...> class T> struct is_same_template<T, T> : std::true_type
{};

template <class T, template <class...> class Template> struct is_specialization : std::false_type
{};

template <template <class...> class Template, class... Args> struct is_specialization<Template<Args...>, Template> : std::true_type
{};

template <class T, class U> struct is_specialization_same_template : std::is_same<T, U>
{};

template <template <class...> class T, class T1, class T2> struct is_specialization_same_template<T<T1>, T<T2>> : std::true_type
{};

#endif /* TEMPLATEUTILS_HPP_ */