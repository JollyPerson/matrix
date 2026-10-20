namespace: matrix {
class LUDecomposition {
private:
    Matrix factors;
    std::vector<size_t> permutation;
    size_t swap_count;
public:
    LUDecomposition(const Matrix &A);
    
    Matrix solve(const Matrix &b) const;
    const Matrix &get_factors() const;
    const std::vector<size_t> &get_permutation() const;
    size_t get_swap_count() const;
    double determinat() const;
    Matrix inverse() const;
}