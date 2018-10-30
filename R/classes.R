#' Buckect
#'
#' @param position 
#' @param t 
#' @param EAD 
#' @param LGDAddon 
#' @param LGD 
#' @param PD 
#' @param rho 
#' @param equation 
#'
#' @return
#' @export

bucketWholesale = function(EAD = EAD, PD = PD, LGD = LGD, rho = rho)
{
  return(new(Bucket, EAD, PD, LGD, rho))
}


