#ifndef MATRIX_H
#define MATRIX_H

template <typename T>
class Matrix
{
public:
    Matrix(int m, int n);
    ~Matrix();

    int m() const { return m_m; }
    int n() const { return m_n; }

    T &at(int m, int n);

private:
    const int m_m, m_n;
    T *m_data;
};

template <typename T>
Matrix<T>::Matrix(int m, int n) : m_m(m), m_n(n)
{
    m_data = new T[m_m * m_n];
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            at(i, j) = 0;
        }
    }
}

template <typename T>
Matrix<T>::~Matrix ()
{
    delete[] m_data;
}

template <typename T>
T  &Matrix<T>::at(int m, int n) {
    return *(m_data + m * this->n() + n);
}


#endif // MATRIX_H
