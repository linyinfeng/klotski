#ifndef MATRIX_H
#define MATRIX_H

template <typename T>
class Matrix
{
public:
    Matrix(int m, int n);
    ~Matrix();

    int m() const { return m_; }
    int n() const { return n_; }

    T &at(int i, int j);

    void clear();

private:
    const int m_, n_;
    T *m_data;
};

template <typename T>
void Matrix<T>::clear() {
    for (int i = 0; i < m_; ++i) {
        for (int j = 0; j < n_; ++j) {
            at(i, j) = 0;
        }
    }
}

template <typename T>
Matrix<T>::Matrix(int m, int n) : m_(m), n_(n)
{
    m_data = new T[m_ * n_];
    clear();
}

template <typename T>
Matrix<T>::~Matrix ()
{
    delete[] m_data;
}

template <typename T>
T &Matrix<T>::at(int i, int j) {
    return *(m_data + i * n() + j);
}

#endif // MATRIX_H
