#include "standardized.h"
#include "plotting.h"
#include <matrix/matrix.h>
#include <assert.h>


void make_related_features_matrices (Matrix * features, Matrix * features_zt1, Matrix * features_mu, Matrix * features_sigma);
void make_related_labels_matrices (Matrix * labels, Matrix * labels_zt, Matrix * labels_mu, Matrix * labels_sigma);


void make_related_features_matrices (Matrix * features, Matrix * features_zt1, Matrix * features_mu, Matrix * features_sigma) {
    size_t nsamples = features->nr;
    size_t nfeatures = features->nc;
    Matrix * features_z = matrix_create(nsamples, nfeatures);
    Matrix * features_zt = matrix_create(nfeatures, nsamples);
    Matrix * ones = matrix_create(1, nsamples);
    matrix_ones(ones);
    matrix_stzdwn(features, features_mu, features_sigma, features_z);
    matrix_transp(features_z, features_zt);
    matrix_vstack(ones, features_zt, features_zt1);
    matrix_destroy(&features_z);
    matrix_destroy(&features_zt);
    matrix_destroy(&ones);
}


void make_related_labels_matrices (Matrix * labels, Matrix * labels_zt, Matrix * labels_mu, Matrix * labels_sigma) {
    size_t nsamples = labels->nr;
    size_t nlabels = labels->nc;
    Matrix * labels_z = matrix_create(nsamples, nlabels);
    matrix_stzdwn(labels, labels_mu, labels_sigma, labels_z);
    matrix_transp(labels_z, labels_zt);
    matrix_destroy(&labels_z);
}


void run_standardized(const size_t nsamples, const size_t nfeatures, const size_t nepochs, const float learning_rate,
                      const char * features_path, const char * labels_path) {

    assert(false && "something is off with the backpropagation");


    // suffixes z: standardized, t: transposed, 1: padded with ones
    Matrix * features = matrix_create(nsamples, nfeatures);
    Matrix * features_zt1 = matrix_create(1 + nfeatures, nsamples);
    Matrix * features_mu = matrix_create(1, nfeatures);
    Matrix * features_sigma = matrix_create(1, nfeatures);
    Matrix * labels = matrix_create(nsamples, 1);
    Matrix * labels_zt = matrix_create(1, nsamples);
    Matrix * labels_mu = matrix_create(1, 1);
    Matrix * labels_sigma = matrix_create(1, 1);

    Matrix * weights = matrix_create(1, 1 + nfeatures);
    Matrix * predicted_t = matrix_create(1, nsamples);
    Matrix * residuals_t = matrix_create(1, nsamples);
    Matrix * residuals_tb = matrix_create(1 + nfeatures, nsamples);
    Matrix * gradients_t = matrix_create(1 + nfeatures, nsamples);
    Matrix * gradient_t = matrix_create(1 + nfeatures, 1);
    Matrix * step_t = matrix_create(1 + nfeatures, 1);
    Matrix * step = matrix_create(1, 1 + nfeatures);

    Matrix * plotting_epochs = matrix_create(1 + nepochs, 1);
    Matrix * plotting_sigma = matrix_create(1 + nepochs, 1);

    matrix_readxs(features_path, features);
    make_related_features_matrices(features, features_zt1, features_mu, features_sigma);
    matrix_print(stdout, "features", features);
    matrix_print(stdout, "features_zt1", features_zt1);
    matrix_print(stdout, "features_mu", features_mu);
    matrix_print(stdout, "features_sigma", features_sigma);

    {
        size_t nrows = matrix_readnr(labels_path);
        size_t ncols = matrix_readnc(labels_path);
        assert(nrows == nsamples && "Expected the number of rows in FEATURES to be "
                                    "the same as the number of rows in LABELS");
        assert(ncols == 1 && "Expected the number of columns in LABELS to be 1");
    }
    matrix_readxs(labels_path, labels);
    make_related_labels_matrices(labels, labels_zt, labels_mu, labels_sigma);
    matrix_print(stdout, "labels", labels);
    matrix_print(stdout, "labels_zt", labels_zt);
    matrix_print(stdout, "labels_mu", labels_mu);
    matrix_print(stdout, "labels_sigma", labels_sigma);

    // ========================== ITERATION ============================== //

    for (size_t i = 0; i <= nepochs; i++) {
        matrix_dotpro(weights, features_zt1, predicted_t);
        matrix_ebesub(predicted_t, labels_zt, residuals_t);
        {
            plotting_epochs->xs[i] = i;
            plotting_sigma->xs[i] = matrix_sdvall(residuals_t);
        }
        matrix_bctdwn(residuals_t, residuals_tb);
        matrix_hadpro(residuals_tb, features_zt1, gradients_t);
        matrix_avgrgt(gradients_t, gradient_t);
        matrix_scapro(gradient_t, learning_rate, step_t);
        matrix_transp(step_t, step);
        matrix_ebesub(weights, step, weights);
    }

    matrix_print(stdout, "weights", weights);
    plot_residuals("qtwidget", plotting_epochs, plotting_sigma, nepochs, nsamples);

    // =================== DEALLOCATE DYNAMIC MEMORY ====================== //

    matrix_destroy(&features);
    matrix_destroy(&features_zt1);
    matrix_destroy(&features_mu);
    matrix_destroy(&features_sigma);
    matrix_destroy(&labels);
    matrix_destroy(&labels_zt);
    matrix_destroy(&labels_mu);
    matrix_destroy(&labels_sigma);

    matrix_destroy(&weights);
    matrix_destroy(&predicted_t);
    matrix_destroy(&residuals_t);
    matrix_destroy(&residuals_tb);
    matrix_destroy(&gradients_t);
    matrix_destroy(&gradient_t);
    matrix_destroy(&step_t);
    matrix_destroy(&step);

    matrix_destroy(&plotting_epochs);
    matrix_destroy(&plotting_sigma);
}
